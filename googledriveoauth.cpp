#include "googledriveoauth.h"

void GoogleDriveOauth::setup() {

  auto config = Config::get();
  if (!config->contains("apis/google_client_secret") ||
      !config->contains("apis/google_client_id") ||
      !config->contains("apis/google_refresh_token")) {
    qWarning() << "Google api config not set.";
    return;
  }

  const QString TOKEN = config->value("apis/google_refresh_token").toString(),
                SECRET = config->value("apis/google_client_secret").toString(),
                ID = config->value("apis/google_client_id").toString();

  google = new QOAuth2AuthorizationCodeFlow(this);
  google->setScope(scopes.join(' '));

  // No need

  // connect(googleOauth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
  //         [this](QUrl url) {
  //           qDebug() << url;
  //         });

  connect(google, &QOAuth2AuthorizationCodeFlow::expirationAtChanged, this,
          [](const QDateTime &expiration) {
            qInfo() << "Token expiration date changed to " << expiration;
          });
  google->setAuthorizationUrl(
      QUrl("https://accounts.google.com/o/oauth2/auth"));
  google->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));
  google->setClientIdentifier(ID);
  google->setClientIdentifierSharedKey(SECRET);
  google->setRefreshToken(TOKEN);

  google->setModifyParametersFunction([](QAbstractOAuth::Stage stage,
                                         QVariantMap *parameters) {
    if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
      // const QByteArray code = parameters->value("code").toByteArray();
      (*parameters)["code"] =
          QUrl::fromPercentEncoding(parameters->value("code").toByteArray());
    }
  });

  auto replyHandler = new QOAuthHttpServerReplyHandler(PORT, this);
  google->setReplyHandler(replyHandler);

  connect(google, &QOAuth2AuthorizationCodeFlow::granted,this, [this]() {
    emit accessUpdate(true);
    qDebug() << __FUNCTION__ << __LINE__ << "Access Granted!";
    qDebug() << google->token();
    qDebug() << google->expirationAt();
  });
}

// NOTE: The io device passed as parameter will be deleted
// when the response is received
void GoogleDriveOauth::upload(QIODevice *picInput, const QString &basename,
                              const QString &mimeType,
                              const QString &token) const {
  // const QString fileName = "...";

  QNetworkRequest req(QUrl("https://www.googleapis.com/"
                           "upload/drive/v3/files?uploadType=multipart"));

  QNetworkAccessManager *man = new QNetworkAccessManager;

  QHttpMultiPart *multipart =
      new QHttpMultiPart(QHttpMultiPart::FormDataType, man);

  // file part

  QHttpPart filePart;

  picInput->setParent(multipart);
  picInput->open(QIODevice::ReadOnly);
  filePart.setBodyDevice(picInput);
  filePart.setHeader(
      QNetworkRequest::ContentDispositionHeader,
      QString("form-data; name=\"file\"; filename=\"%1\"").arg(basename));

  filePart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType);

  // meta part

  QHttpPart metaPart;
  metaPart.setBody(QString("{\"name\": \"%1\"}").arg(basename).toUtf8());
  metaPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                     "form-data; name=\"metadata\"");
  metaPart.setHeader(QNetworkRequest::ContentTypeHeader,
                     "application/json;charset=UTF-8");

  // append to form

  multipart->append(metaPart);
  multipart->append(filePart);

  // set token

  req.setRawHeader("Authorization", "Bearer " + token.toUtf8());

  QNetworkReply *reply = man->post(req, multipart);

  QNetworkReply::connect(reply, &QNetworkReply::finished, this, [=]() {
    qDebug().noquote() << reply->readAll();
    man->deleteLater();
  });
}

void GoogleDriveOauth::upload(QFile *picFile, const QString &token) const {

  const QString basename = QFileInfo(*picFile).baseName();

  const QString mimeType = QMimeDatabase().mimeTypeForFile(*picFile).name();

  upload(picFile, basename, mimeType, token);
}

bool GoogleDriveOauth::tokenIsValid() const {
  return google->expirationAt().isValid() &&
         google->expirationAt().addSecs(3600) < QDateTime::currentDateTime();
}
