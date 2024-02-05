#include "dialog.h"
#include "ui_dialog.h"
#include <QSslSocket>
#include "smtp.h"
#include <qfiledialog.h>
#include <QMessageBox>



Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_sendmailbtn_clicked()
{  qDebug()<< "Hi" ;
     if(QSslSocket::supportsSsl()) {
        qDebug() << "SSL/TLS is supported.";
    } else {
        qDebug() << "SSL/TLS is not supported.";
    }
    Smtp* smtp = new Smtp("pointi.the.site@gmail.com","sjagyrazuyjcdckr", "smtp.gmail.com");
    connect(smtp, SIGNAL(status(QString)), this, SLOT(mailSent(QString)));

    if( !files.isEmpty() )
        smtp->sendMail("pointi.the.site@gmail.com", ui->rcpt->text() , ui->subject->text(),ui->msg->toPlainText(), files );
    else
        smtp->sendMail("pointi.the.site@gmail.com", ui->rcpt->text() , ui->subject->text(),ui->msg->toPlainText());
}


void Dialog::on_browsemailbtn_clicked()
{files.clear();

    QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (dialog.exec())
        files = dialog.selectedFiles();

    QString fileListString;
    foreach(QString file, files)
        fileListString.append( "\"" + QFileInfo(file).fileName() + "\" " );

    ui->file->setText( fileListString );

}
void Dialog::mailSent(QString status)
{

    if(status == "Message sent")
        QMessageBox::warning( nullptr, tr( "Qt Simple SMTP client" ), tr( "Message sent!\n\n" ) );
    ui->rcpt->clear();
    ui->subject->clear();
    ui->file->clear();
    ui->msg->clear();
    //ui->mail_pass->clear();


}
