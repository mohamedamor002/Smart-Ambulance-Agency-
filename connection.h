#ifndef CONNECTION_H
#define CONNECTION_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <map>

class Connection {

public:
  ~Connection();
  QSqlDatabase db;
  // Connection() = default;
  bool create();
  // void close();
};

#endif // CONNECTION_H
