#include "connection.h"
#include "config.h"

bool Connection::create() {
  auto conf = Config::get();
  db = QSqlDatabase::addDatabase("QODBC");
  for (const auto &[key, method] :
       std::map<QString, void (QSqlDatabase::*)(const QString &)>{
           {"database/name", &QSqlDatabase::setDatabaseName},
           {"database/username", &QSqlDatabase::setUserName},
           {"database/password", &QSqlDatabase::setPassword}})
    if (conf->contains(key))
      (db.*method)(conf->value(key).toString());

  return db.open();
}
// void Connection::close() { db.close(); }

Connection::~Connection() { db.close(); }
