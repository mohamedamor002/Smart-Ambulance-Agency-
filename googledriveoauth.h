#ifndef GOOGLEDRIVEOAUTH_H
#define GOOGLEDRIVEOAUTH_H

#include "config.h"
#include <QFile>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QtGlobal>
#include <QtNetwork>
#include <atomic>

class GoogleDriveOauth : public QObject {
  Q_OBJECT
private:
  uint PORT = 3000;
  QStringList scopes{
      "https://www.googleapis.com/auth/drive",
      // "email",
  };

public:
  GoogleDriveOauth() = default;
  // GoogleDriveOauth(GoogleDriveOauth &&) = default;
  // GoogleDriveOauth(const GoogleDriveOauth &) = default;
  // GoogleDriveOauth &operator=(GoogleDriveOauth &&) = default;
  // GoogleDriveOauth &operator=(const GoogleDriveOauth &) = default;
  ~GoogleDriveOauth() = default;

  QOAuth2AuthorizationCodeFlow *google = nullptr;
  void setup();
  void upload(QIODevice *picInput, const QString &basename,
              const QString &mimeType, const QString &token) const;
  void upload(QFile *picFile, const QString &token) const;
  bool tokenIsValid() const;
  // std::atomic_bool authenticated = false;

signals:
  void accessUpdate(bool granted);
};

#endif // GDRIVEOAUTH_H
