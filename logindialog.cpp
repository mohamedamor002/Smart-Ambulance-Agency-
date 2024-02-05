#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog) {
  ui->setupUi(this);

  ui->usernameLineEdit->setFocus();

  ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

void LoginDialog::done(int choice) {

  // Default behavior if user cancelled

  if (choice != QDialog::Accepted) {
    QDialog::done(choice);
    return;
  }

  // Empty fields

  if (ui->usernameLineEdit->text().isEmpty() ||
      ui->passwordLineEdit->text().isEmpty()) {
    QMessageBox::warning(this, tr("Missing Fields"),
                         tr("Please fill in both username and password."));
    return;
  }

  if (auto member = Staff::find(ui->usernameLineEdit->text());
      !member.has_value()) {
    QMessageBox::warning(this, tr("Login Failed"), tr("No such user exists"));
  } else if (Account::hashPassword(ui->passwordLineEdit->text()) !=
             member.value().account->getPassword()) {
    QMessageBox::warning(this, tr("Login Failed"),
                         tr("Password and username mismatch"));
  } else {

    // Success

    emit userLoggedIn(member.value());

    QDialog::done(choice);
  }
}

std::optional<Staff> LoginDialog::login() {
  std::optional<Staff> member = std::nullopt;

  connect(this, &LoginDialog::userLoggedIn, this,
          [&member](const Staff &loggedInmember) { member = loggedInmember; });

  exec();

  return member;
}

LoginDialog::~LoginDialog() { delete ui; }
