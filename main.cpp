#include "connection.h"
#include "logindialog.h"
#include "mainwindow.h"
#include "staff.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[]) {

  Connection conn;

  if (!conn.create()) {
    qDebug() << "Failed to connect to database: " << conn.db.lastError();
    return 1;
  }

  QApplication a(argc, argv);

  LoginDialog dialog;

  // login disabled for now, TODO: uncomment

  const std::optional<Staff> member = dialog.login();
  // const std::optional<Staff> member = Staff();

  if (!member.has_value())
    return 1;

  MainWindow w(nullptr, member.value());

  w.showMaximized();
  w.show();

  return a.exec();
}
