#include "account.h"

Account::Account(QString username, QString passwordEncrypted, QString status)
    : username(username), passwordEncrypted(passwordEncrypted), status(status) {
}

Account::Account(QSqlQuery query)
    : Account(query.value("USERNAME").toString(),
              query.value("PASSWORD").toString(),
              query.value("STATUS").toString()) {}

// hashing

// HACK: the algorithms provided by QCryptographicHash are not secure enough to
// be used for passwords

// TODO: generate salt

QString Account::hashPassword(const QString &nonEncryptedPassword) {
  return QCryptographicHash::hash(nonEncryptedPassword.toUtf8(),
                                  QCryptographicHash::Keccak_512).toBase64();
}

bool Account::validatePassword(const QString &nonEncryptedPassword,
                               const QString &encryptedPassword) {
  return hashPassword(nonEncryptedPassword) == encryptedPassword;
}
QString Account::generatePassword() {

  // available characters to generate a password from

  constexpr quint8 len = 63;
  constexpr char available[len] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  constexpr quint8 min = 10, max = 16;

  const quint8 randomPasswordLen =
      QRandomGenerator::global()->bounded(min, max);
  QString password;

  for (quint8 i = 0; i < randomPasswordLen; ++i)
    password += available[QRandomGenerator::global()->bounded(len)];

  // qDebug() << password;

  return password;

  // QRandomGenerator lenGen;
  // lenGen.generate(10, 16);

  // constexpr const quint32 chosen[]{
  //     'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  //     'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  //     'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  //     'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  //     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  // };

  // QVector<quint32> vec;
  // vec.resize(10);
  // QRandomGenerator::global()->fillRange(vec.data(), 10);
  // qDebug() << vec;

  // QRandomGenerator x(chosen, chosen + len);
  // x.seed();
  // qDebug() << x.bounded(20);
}

QString Account::generateUsername(QString firstName, QString lastName) {

  return firstName + (lastName.isEmpty() ? 'X' : lastName.at(0)) + '_' +
         QString::number(QRandomGenerator::global()->bounded(100, 999));
}
