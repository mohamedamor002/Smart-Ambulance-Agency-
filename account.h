#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QCryptographicHash>
#include <QDebug>
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <optional>

// Database definition

// AC_ID
// USERNAME
// PASSWORD
// ROLE
// STATUS
// S_ID

class Account {

  static inline QCryptographicHash hash =
      QCryptographicHash(QCryptographicHash::Md5);
  QString username, passwordEncrypted;
  QString status;

public:
  Account() = default;
  Account(QString username, QString passwordEncrypted,
          QString status = QString());

  Account(QSqlQuery query);
  Account(Account &&) = default;
  Account(const Account &) = default;
  Account &operator=(Account &&) = default;
  Account &operator=(const Account &) = default;
  ~Account() = default;

  // accessors

  QString getUserName() const { return username; }
  QString getPassword() const { return passwordEncrypted; }
  QString getStatus() const { return status; }

  // mutators

  void setUserName(QString username) { this->username = username; }
  void setPassword(QString passwordEncrypted) {
    this->passwordEncrypted = passwordEncrypted;
  }
  void setStatus(QString status) { this->status = status; }

  // hashing

  static QString hashPassword(const QString &nonEncryptedPassword);
  static bool validatePassword(const QString &nonEncryptedPassword,
                               const QString &encryptedPassword);
  static QString generatePassword();
  static QString generateUsername(QString firstName, QString lastName);

  // TODO: password and username generation functions to be used for new users
};

#endif // ACCOUNT_H
