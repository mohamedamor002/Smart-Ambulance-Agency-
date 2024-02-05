#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "staff.h"
#include <QAbstractButton>
#include <QDialog>
#include <QMessageBox>
#include <optional>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
  Q_OBJECT

public:
  explicit LoginDialog(QWidget *parent = nullptr);
  ~LoginDialog();
  std::optional<Staff> login();
signals:
  void userLoggedIn(const Staff &member);

private:
  Ui::LoginDialog *ui;

  void done(int);
};

#endif // LOGINDIALOG_H
