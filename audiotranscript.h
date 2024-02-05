#ifndef AUDIOTRANSCRIPT_H
#define AUDIOTRANSCRIPT_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QWebSocket>
#include <QtEndian>
#include <QtMultimedia>
#include <QtNetwork>
#include <functional>

class AudioTranscript : public QObject {
  Q_OBJECT
public:
  enum class Api { Request, WebSocket } apiMode = Api::Request;
  AudioTranscript(AudioTranscript::Api apiMode = Api::Request,
                  QObject *parent = nullptr);
  // AudioTranscript(AudioTranscript &&) = default;
  // AudioTranscript(const AudioTranscript &) = default;
  // AudioTranscript &operator=(AudioTranscript &&) = default;
  // AudioTranscript &operator=(const AudioTranscript &) = default;
  ~AudioTranscript() = default;
  // void closeBuffer();
  QAudioFormat format() const { return m_format; };
  static void
  retrieveTranscriptFromWav(const QByteArray &wavAudio,
                            std::function<void(QNetworkReply *)> callback);
  static QByteArray rawToWav(const QByteArray &rawAudio,
                             const QAudioFormat &format);

  static constexpr const char *DEEPGRAM_ENDPOINT = "api.deepgram.com/v1/listen";

  QString fromResponse(const QByteArray &response) const;
public slots:
  void startRecording();
  void stopRecording();

signals:
  void recordedSession(const QByteArray &audioData);
  void recordedChunk(const QByteArray &audioData);
  void receivedFromSocket(const QString &response);

private:
  QAudioFormat m_format;
  QAudioInput *input = nullptr;
  QBuffer *audioBuffer = nullptr;
  QNetworkRequest socketRequest;

  // QByteArray data{};

  QWebSocket *m_webSocket = nullptr;

  void setupBuffering();
  void configureSocket();

  // QByteArray rawToWav(const QByteArray &rawData) {
  //   return rawToWav(rawData, m_format);
  // }
};

#endif // AUDIOTRANSCRIPT_H
