#include "audiotranscript.h"
#include "config.h"

AudioTranscript::AudioTranscript(Api apiMode, QObject *parent)
    : QObject(parent), apiMode(apiMode) {

  // Configure the format

  m_format.setSampleRate(16000);
  m_format.setChannelCount(1);
  m_format.setSampleSize(16);
  m_format.setCodec("audio/pcm");
  m_format.setSampleType(QAudioFormat::SignedInt);
  m_format.setByteOrder(QAudioFormat::LittleEndian);

  // Setup audio input and its buffer

  input = new QAudioInput(m_format, this);
  audioBuffer = new QBuffer(input);

  // Web socket specific settings

  if (apiMode == Api::WebSocket) {

    const QString TOKEN = Config::get()->value("apis/deepgram_key").toString();

    if (TOKEN.isEmpty()) {
      qWarning() << "Transcript: API key not set";
      return;
    }

    // input->setNotifyInterval(128);

    setupBuffering();

    QUrl url(QString("wss://") + DEEPGRAM_ENDPOINT);
    const QUrlQuery query{
        {"model", "nova"},
        {"language", "en-US"},
        {"punctuate", "true"},
        {"numerals", "true"},
        {"utterances", "true"},
        {"diarize", "true"},
        // Specify the format since we're sending raw audio
        {"sample_rate", QString::number(m_format.sampleRate())},
        {"encoding", "linear16"}, // In the case of 16 bit little endian
        {"channels", QString::number(m_format.channelCount())},
    };

    url.setQuery(query);

    socketRequest = QNetworkRequest(url);

    socketRequest.setRawHeader("Authorization",
                               (QString("Token ") + TOKEN).toUtf8());

    m_webSocket = new QWebSocket;
    m_webSocket->setParent(this);
    configureSocket();
  }
}

// Convert the raw pcm audio to wav format.
// This is needed if we're submitting the audio via http

QByteArray AudioTranscript::rawToWav(const QByteArray &rawData,
                                     const QAudioFormat &format) {
  QByteArray result;

  // Prepare the WAV header
  const qint32 fileSize =
      rawData.size() + 36; // 36 = size of fmt + size of data headers
  const qint32 fmtSize = 16;
  const qint16 audioFormat = 1; // PCM
  const qint16 channelCount = format.channelCount();
  const qint32 sampleRate = format.sampleRate();
  const qint32 byteRate =
      format.sampleRate() * channelCount * format.sampleSize() / 8;
  const qint16 blockAlign = channelCount * format.sampleSize() / 8;
  const qint16 bitsPerSample = format.sampleSize();
  const qint32 dataSize = rawData.size();

  // Append all the data

  result.append("RIFF", strlen("RIFF"));

  result.append(reinterpret_cast<const char *>(&fileSize), sizeof fileSize);

  result.append("WAVE", strlen("WAVE"));
  result.append("fmt ", strlen("fmt "));

  result.append(reinterpret_cast<const char *>(&fmtSize), sizeof fmtSize);
  result.append(reinterpret_cast<const char *>(&audioFormat),
                sizeof audioFormat);
  result.append(reinterpret_cast<const char *>(&channelCount),
                sizeof channelCount);
  result.append(reinterpret_cast<const char *>(&sampleRate), sizeof sampleRate);
  result.append(reinterpret_cast<const char *>(&byteRate), sizeof byteRate);
  result.append(reinterpret_cast<const char *>(&blockAlign), sizeof blockAlign);
  result.append(reinterpret_cast<const char *>(&bitsPerSample),
                sizeof bitsPerSample);

  result.append("data", strlen("data"));

  result.append(reinterpret_cast<const char *>(&dataSize), sizeof dataSize);

  // Append the raw audio data
  result.append(rawData);

  return result;
}

// Given wav binary data, we post it to the api and retrieve the full transcript
void AudioTranscript::retrieveTranscriptFromWav(
    const QByteArray &wavData, std::function<void(QNetworkReply *)> callback) {
  QNetworkAccessManager *man = new QNetworkAccessManager();

  const QString TOKEN = Config::get()->value("apis/deepgram_key").toString();

  if (TOKEN.isEmpty()) {
    qWarning() << "Transcript: API key not set";
    return;
  }

  QUrl url = QUrl(QString("https://") + DEEPGRAM_ENDPOINT);

  const QUrlQuery query{
      {"model", "nova"},    {"language", "en-US"},  {"punctuate", "true"},
      {"numerals", "true"}, {"utterances", "true"}, {"diarize", "true"},
  };

  url.setQuery(query);

  QNetworkRequest req(url);

  req.setHeader(QNetworkRequest::ContentTypeHeader,
                "application/x-www-form-urlencoded");

  req.setRawHeader("Authorization", (QString("Token ") + TOKEN).toUtf8());

  QNetworkReply *reply = man->post(req, wavData);

  connect(reply, &QNetworkReply::finished, man, [man, reply, callback]() {
    callback(reply);
    man->deleteLater();
  });
}

void AudioTranscript::startRecording() {

  // Already started
  if (input->state() == QAudio::ActiveState) {
    return;
  }

  // TODO: maybe have this outside of the function

  if (apiMode == Api::WebSocket) {
    m_webSocket->open(socketRequest);
  }

  // Clear the previous data from the buffer
  audioBuffer->open(QIODevice::ReadWrite);

  // Start recording audio
  input->start(audioBuffer);

  qDebug() << "Transcript: Started recording";
}

void AudioTranscript::stopRecording() {

  // Already stopped

  if (input->state() == QAudio::StoppedState)
    return;

  // Stop recording audio
  input->stop();

  // TODO: maybe have this outside of the function

  if (apiMode == Api::Request) {
    // Retrieve the recorded audio data
    const QByteArray audioData = audioBuffer->data();

    emit recordedSession(audioData);
  } else if (apiMode == Api::WebSocket) {

    // m_webSocket->sendTextMessage("{\"type\":\"ClosConnection\"}");

    m_webSocket->close();
  }

  // Clear and close the buffer
  audioBuffer->buffer().clear();
  audioBuffer->close();

  qDebug() << "Transcript: Stopped recording";
}

// void AudioTranscript::onStateChanged(QAudio::State state)
// {
//     if (state == QAudio::StoppedState) {
//         // Stop and close the buffer when the audio input is stopped
//         stopRecording();
//     }
// }

// Configure the audio recording to wipe the buffer for every new chunk.
// This is used to allow for streaming data to the websocket
void AudioTranscript::setupBuffering() {

  // connect(input, &QAudioInput::notify, this, [=]() {

  connect(audioBuffer, &QIODevice::readyRead, this, [this]() {
    // qDebug() << audioBuffer->buffer().size();

    // Emit the signal

    emit recordedChunk(audioBuffer->buffer());

    // Clear the buffer everytime
    audioBuffer->close();
    audioBuffer->buffer().clear();
    audioBuffer->open(QIODevice::ReadWrite);

    // qDebug() << audioBuffer->buffer().size();
  });
}

// Setup the necessary signal connections for the socket
void AudioTranscript::configureSocket() {

  // connections, disconnections, erros

  connect(m_webSocket, &QWebSocket::connected, this,
          [=]() { qDebug() << "Transcipt: connected to websocket"; });
  connect(m_webSocket, &QWebSocket::disconnected, this,
          [=]() { qDebug() << "Transcipt: disconnected from websocket"; });
  connect(m_webSocket,
          qOverload<QAbstractSocket::SocketError>(&QWebSocket::error),
          [=](QAbstractSocket::SocketError error) { qDebug() << error; });

  // TODO: maybe remove this and forward to a signal instead
  connect(this, &AudioTranscript::recordedChunk, this,
          [=](const QByteArray &chunk) {
            // qDebug() << "sending chunk to websocket" << chunk.size();

            m_webSocket->sendBinaryMessage(chunk);
          });

  // Responses

  // Forward the textMessageReceived signal
  connect(m_webSocket, &QWebSocket::textMessageReceived, this,
          [=](const QString &message) {
            // qDebug() << fromResponse(message.toUtf8());
            emit receivedFromSocket(message);
          });
}

// Given a Json response from the http or websocket api,
// we extract and return the transcript string
QString AudioTranscript::fromResponse(const QByteArray &response) const {

  const QJsonObject jsonResp = QJsonDocument::fromJson(response).object();

  // qDebug() << jsonResp;

  switch (apiMode) {
  case Api::Request:
    return jsonResp["results"]["channels"][0]["alternatives"][0]["transcript"]
        .toString();
  case Api::WebSocket:
    return jsonResp["channel"]["alternatives"][0]["transcript"].toString();
  default:
    return QString();
  };
}
