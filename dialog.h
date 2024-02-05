#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:


    void on_sendmailbtn_clicked();

    void on_browsemailbtn_clicked();
    void mailSent(QString);


private:
    Ui::Dialog *ui;
    QStringList files;

};

#endif // DIALOG_H
