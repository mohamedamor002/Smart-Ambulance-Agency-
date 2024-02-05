#include "ambulance.h"
#include "arduino.h"
#include <QMessageBox>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QPrinter>
#include <QPainter>
#include <QPageSize>
#include <QPageLayout>
#include <QFileDialog>
#include <QPrintDialog>
#include <QDebug>
#include <QObject>
#include <QPdfWriter>
#include <QDesktopServices>
#include <QtGui/QTextDocument>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QTextDocument>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtSql>
#include <QCoreApplication>
#include <QApplication>
#include <QTranslator>
ambulance::ambulance(){}
ambulance::ambulance(int A_id,int reg_number,QString t, QString man, int year, QString fueltype)
{
  this->A_id=A_id;
  this->reg_number=reg_number;
  this->type=t;
  this->manufacturer=man;
  this->year=year;
  this->fueltype=fueltype;
}
bool ambulance::Create()
{
    QSqlQuery query;
    QString id = QString::number(A_id);
    QString y = QString::number(year);
    QString reg = QString::number(reg_number);

    query.prepare("INSERT INTO Ambulance (A_ID, REG_NUMBER, TYPE, MANUFACTURER, YEAR, FUELTYPE)" "VALUES (:A_id, :reg_number, :type, :manufacturer, :year, :fueltype)");

    query.bindValue(":A_id",id);
    query.bindValue(":reg_number",reg);
    query.bindValue(":type",type);
    query.bindValue(":manufacturer",manufacturer);
    query.bindValue(":year",y);
    query.bindValue(":fueltype",fueltype);

    return query.exec();
}
bool ambulance::Update()
{



    QString id = QString::number(A_id);
    QString y = QString::number(year);
    QString reg = QString::number(reg_number);
    QSqlQuery qry;
    qry.prepare("UPDATE Ambulance SET REG_NUMBER=:reg,TYPE=:type,MANUFACTURER=:manufacturer,YEAR=:year,FUELTYPE=:fueltype WHERE A_ID=:id");
    qry.bindValue(":id", id);
    qry.bindValue(":reg", reg);
    qry.bindValue(":type", type);
    qry.bindValue(":manufacturer", manufacturer);
    qry.bindValue(":year", y);
    qry.bindValue(":fueltype", fueltype);

    return qry.exec();
}
bool ambulance::Delete(QString A_id)
{
    QSqlQuery query;


    query.prepare("DELETE FROM Ambulance WHERE A_ID='"+A_id+"'");

    return query.exec();
}

QSqlQueryModel * ambulance::Read()
{
    QSqlQueryModel * model=new QSqlQueryModel();
    model->setQuery("select * from Ambulance");
    model->setHeaderData(0,Qt::Horizontal,QObject::tr("A_id"));
    model->setHeaderData(1,Qt::Horizontal,QObject::tr("reg_number"));
    model->setHeaderData(2,Qt::Horizontal,QObject::tr("type"));
    model->setHeaderData(3,Qt::Horizontal,QObject::tr("manufacturer"));
    model->setHeaderData(4,Qt::Horizontal,QObject::tr("year"));
    model->setHeaderData(5,Qt::Horizontal,QObject::tr("fueltype"));
    return model;
}
QSqlQueryModel * ambulance::Sort(const QString &arg1){

    QSqlQueryModel *model = new QSqlQueryModel();
        QString query;

        if (arg1 == "Date of Manufacturing") {
            query = "SELECT * FROM AMBULANCE ORDER BY YEAR ";
        } else if (arg1 == "Fuel type") {
            query = "SELECT * FROM AMBULANCE ORDER BY FUELTYPE ";
        } else if (arg1 == "Model") {
            query = "SELECT * FROM AMBULANCE ORDER BY MANUFACTURER ";
        }

        model->setQuery(query);
        return model;
}
QSqlQueryModel * ambulance::Search(int id){
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM AMBULANCE WHERE A_ID = " + QString::number(id));
    return model;
}

QString ambulance::generatePDF()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr, QCoreApplication::translate("C:/Users/kharr/OneDrive/Bureau", "C:/Users/kharr/OneDrive/Bureau"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    QPdfWriter pdf(dir + "/PdfList.pdf");
    QPainter painter(&pdf);
    int i = 4000;

    painter.drawText(900, 650, "Smart Ambulance Agency");

    painter.setPen(Qt::red);
    painter.setFont(QFont("Times New Roman", 25));
    painter.drawText(3000, 1400, "List of Ambulance");
    painter.setPen(Qt::black);
    painter.setFont(QFont("Times New Roman", 15));
    painter.drawRect(100, 100, 9400, 2500);
    painter.drawRect(100, 3000, 9400, 500);
    painter.setFont(QFont("Times New Roman", 9));
    painter.drawText(300, 3300, "ID");
    painter.drawText(2300, 3300, "Registration N°");
        painter.drawText(4300, 3300, "Type");
    painter.drawText(6300, 3300, "Manufacturer");
    painter.drawText(7500, 3300, "year");
    painter.drawText(8500, 3300, "Fuel type");

    painter.drawRect(100, 3000, 9400, 10700);

    QSqlQuery query;
    query.prepare("select * from Ambulance");
    query.exec();
    while (query.next())
    {
            painter.drawText(300, i, query.value(0).toString());
            painter.drawText(2300, i, query.value(1).toString());
            painter.drawText(4300, i, query.value(2).toString());
            painter.drawText(6300, i, query.value(3).toString());
            painter.drawText(7500, i, query.value(4).toString());
            painter.drawText(8500, i, query.value(5).toString());

            i += 500;
    }

    int response = QMessageBox::question(nullptr, "Generate PDF", "<PDF saved>...Do you want to display the file?", QMessageBox::Yes | QMessageBox::No);
    if (response == QMessageBox::Yes)
    {
            QDesktopServices::openUrl(QUrl::fromLocalFile(dir + "/PdfList.pdf"));
    }

    painter.end();

    return dir + "/PdfList.pdf";
}
