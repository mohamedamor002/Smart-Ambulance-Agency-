#include "bill.h"
#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QBarSet>
#include <QBarSeries>
#include <QChart>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QChartView>
#include <QDate>
#include <QDebug>
#include <QSqlError>
#include<QIcon>
#include <QSystemTrayIcon>
#include <QPainter>
#include<QPrinter>
#include <QSslSocket>



bill:: bill(){}
bill:: bill(QString fname , QString lname ,QString insurance ,QString phone,QString type , int h_id ,int b_id , int p_id ,int state,QString  value )
{

this->h_id=h_id ;
this->fname=fname ;
this->lname=lname ;
this->insurance= insurance ;
this->type= type ;
this-> phone=phone ;
this ->state=state ;
this->b_id=b_id ;
this->p_id=p_id ;
    this ->value=value ;

}
bool bill:: create()
{
   QSqlQuery  query;

   QString b_id1 = QString::number(b_id);
   QString h_id1 = QString::number(h_id);
   QString p_id1 = QString::number(p_id);
   QString state1 = QString::number(state);
  // QString value1= QString::number(value);

    //prepare() takes the query as a parameter to prepare it for execution
    query.prepare("insert into BILL (B_ID,FNAME,LNAME,PHONE_PATIENT,EMER_TYPE,A_ID,INSURANCE,BILL_STATE,VALUE,H_ID) values (:B_ID,:FNAME,:LNAME,:PHONE_PATIENT,:EMER_TYPE,:A_ID,:INSURANCE,:BILL_STATE,:VALUE,:H_ID)");

    // Creating variables
    query.bindValue(":B_ID",b_id1);
    query.bindValue(":FNAME",fname);
    query.bindValue(":LNAME",lname);
    query.bindValue(":PHONE_PATIENT",phone);
    query.bindValue(":EMER_TYPE",type);
    query.bindValue(":A_ID",p_id1);
    query.bindValue(":INSURANCE",insurance);
    query.bindValue(":BILL_STATE",state1);
    query.bindValue(":VALUE",value );
    query.bindValue(":H_ID",h_id1);

    return query.exec();
}





QSqlQueryModel * bill::read()
{
    QSqlQueryModel * model=new QSqlQueryModel();

    model->setQuery("select * from bill");
    model->setHeaderData(0,Qt::Horizontal,QObject::tr("ID"));
    model->setHeaderData(1,Qt::Horizontal,QObject::tr("FNAME"));
    model->setHeaderData(2,Qt::Horizontal,QObject::tr("LNAME"));
    model->setHeaderData(3,Qt::Horizontal,QObject::tr("PHONE_PATIENT"));
    model->setHeaderData(4,Qt::Horizontal,QObject::tr("EMER_TYPE"));
    model->setHeaderData(5,Qt::Horizontal,QObject::tr("Emergency Id"));
    model->setHeaderData(6,Qt::Horizontal,QObject::tr("INSURANCE"));
    model->setHeaderData(7,Qt::Horizontal,QObject::tr("BILL_STATE"));
    model->setHeaderData(8,Qt::Horizontal,QObject::tr("VALUE"));
    model->setHeaderData(9,Qt::Horizontal,QObject::tr("Hospital Id"));

    return  model;
}
bool bill::Delete(int id)
{
    QSqlQuery query;
    QString res=QString::number(id);
    query.prepare("Delete from bill where B_ID= :id");

    query.bindValue(":id",res);

    return  query.exec();

}




bool bill::Update()
{

    QSqlQuery  query;

    QString b_id1 = QString::number(b_id);
    QString h_id1 = QString::number(h_id);
    QString p_id1 = QString::number(p_id);
    QString state1 = QString::number(state);
   // QString value1= QString::number(value);

     //prepare() takes the query as a parameter to prepare it for execution
     query.prepare("Update  BILL set  B_ID=:B_ID,FNAME=:FNAME,LNAME=:LNAME,PHONE_PATIENT=:PHONE_PATIENT,EMER_TYPE=:EMER_TYPE,A_ID=:A_ID,INSURANCE=:INSURANCE,BILL_STATE=:BILL_STATE,VALUE=:VALUE,H_ID=:H_ID  WHERE B_ID=:B_ID");

     // Creating variables
     query.bindValue(":B_ID",b_id1);
     query.bindValue(":FNAME",fname);
     query.bindValue(":LNAME",lname);
     query.bindValue(":PHONE_PATIENT",phone);
     query.bindValue(":EMER_TYPE",type);
     query.bindValue(":A_ID",p_id1);
     query.bindValue(":INSURANCE",insurance);
     query.bindValue(":BILL_STATE",state1);
     query.bindValue(":VALUE",value);
     query.bindValue(":H_ID",h_id1);

     return query.exec();




//   query.prepare("UPDATE hospital SET PHONE=:phone, S_ID=:s_id, NBR_BEDS=:nb_beds, NBR_PATIENTS=:nb_patients, ADDRESS=:adress, TYPE=:type, HNAME=:name, SPECIALITY=:speciality, EMAIL=:email  WHERE H_ID=:id ");


}


QSqlQuery bill :: GetForeignDetails(int p_id)

{              QSqlQuery query ;

     query.prepare("SELECT EMERGENCY.CURRENT_STATE, EMERGENCY.DURATION, EMERGENCY.PATHOLOGY_TYPES, EMERGENCY.REGULATION_DECISION, AMBULANCE.TYPE FROM EMERGENCY JOIN AMBULANCE ON EMERGENCY.A_ID = AMBULANCE.A_ID WHERE EMERGENCY.EM_ID = ?");
     query.bindValue(0, p_id); // bind p_id to the query parameter


     return query  ;
}
QSqlQueryModel *bill::  search(QString s)
{


     QSqlQuery  query;
     query.prepare("SELECT * FROM bill WHERE FNAME LIKE '%" + s + "%' OR B_ID LIKE '%" + s + "%'");
     query.exec() ;


     QSqlQueryModel *model = new QSqlQueryModel();
     model->setQuery(query);



     return model ;


}
QSqlQueryModel * bill::sort(QString o)
{
     if(o=="Id") o="B_ID" ;
     else if (o=="Value") o="VALUE"  ;
     QSqlQueryModel * model=new QSqlQueryModel();

     model->setQuery("select * from bill ORDER BY "+ o);
     model->setHeaderData(0,Qt::Horizontal,QObject::tr("ID"));
     model->setHeaderData(1,Qt::Horizontal,QObject::tr("FNAME"));
     model->setHeaderData(2,Qt::Horizontal,QObject::tr("LNAME"));
     model->setHeaderData(3,Qt::Horizontal,QObject::tr("PHONE_PATIENT"));
     model->setHeaderData(4,Qt::Horizontal,QObject::tr("EMER_TYPE"));
     model->setHeaderData(5,Qt::Horizontal,QObject::tr("Emergency Id"));
     model->setHeaderData(6,Qt::Horizontal,QObject::tr("INSURANCE"));
     model->setHeaderData(7,Qt::Horizontal,QObject::tr("BILL_STATE"));
     model->setHeaderData(8,Qt::Horizontal,QObject::tr("VALUE"));
     model->setHeaderData(9,Qt::Horizontal,QObject::tr("Hospital Id"));

     return  model;

}
void bill ::exportBill(int id,QString fileName)
{
     qDebug()<< "hi" ;
     // Execute a query to retrieve the bill information by ID
     QSqlQuery query;
     query.prepare("SELECT * FROM bill WHERE B_ID = ?");
     query.addBindValue(id);

     if (!query.exec())
     {
         qDebug() << "Failed to execute query: " << query.lastError().text();
         return;
     }
     qDebug()<< "hi2" ;

     // If the query returned a result, save the bill information to a PDF file
     if (query.next())
     {

         if (!fileName.isEmpty())
         {
             QPrinter printer(QPrinter::HighResolution);
             printer.setOutputFormat(QPrinter::PdfFormat);
             printer.setOutputFileName(fileName);


             QPainter painter(&printer);
             painter.setRenderHint(QPainter::Antialiasing);

             // Set the font, size, and color for the text
             QFont titleFont("Calibri", 24, QFont::Bold);
             QFont normalFont("Arial", 16);
             painter.setFont(titleFont);
             painter.setPen(QColor("#0072C6"));
             // Draw the bill information onto the painter
             QString fname = query.value("FNAME").toString();
             QString lname = query.value("LNAME").toString();
             QString phone = query.value("PHONE_PATIENT").toString();
             QString type = query.value("EMER_TYPE").toString();
             QString insurance = query.value("INSURANCE").toString();
             // int h_id = query.value("H_ID").toInt();
             // int p_id = query.value("P_ID").toInt();

             //int state = query.value("STATE").toInt();
             QString value = query.value("VALUE").toString();

             painter.drawText(3000, 700, "Bill Information");

             int x = 800;
             int y = 2000;

             painter.setFont(normalFont);
             painter.setPen(Qt::black);

             painter.drawText(x, y, "First Name:");
             painter.setPen(QColor("#0072C6"));
             painter.drawText(x + 1600, y, fname);
             painter.setPen(Qt::black);

             y += 500;
             painter.drawText(x, y, "Last Name:");
             painter.setPen(QColor("#0072C6"));
             painter.drawText(x + 1600, y, lname);
             painter.setPen(Qt::black);

             y += 500;
             painter.drawText(x, y, "Phone:");
             painter.setPen(QColor("#0072C6"));
             painter.drawText(x + 1600, y, phone);
             painter.setPen(Qt::black);

             y += 500;
             painter.drawText(x, y, "Type:");
             painter.setPen(QColor("#0072C6"));
             painter.drawText(x + 1600, y, type);
             painter.setPen(Qt::black);

             y += 500;
             painter.drawText(x, y, "Insurance:");
             painter.setPen(QColor("#0072C6"));
             painter.drawText(x + 1600, y, insurance);
             painter.setPen(Qt::black);
             y += 500;

             painter.drawText(x, y, "Total value:");
             painter.setPen(QColor("#0072C6"));
             painter.drawText(x + 1600, y, value);
             painter.setPen(Qt::black);


             y += 5000;

             painter.drawText(x+5000, y, "Signature here:");
             painter.setPen(QColor("#0072C6"));
             //painter.drawText(x + 1600, y, value);
             painter.setPen(Qt::black);




             // Finish painting and close the PDF file
             painter.end();


         }
     }



     qDebug()<< "hi3" ;





}

void bill::statBill()
{

     // Total bill amount
     float total = 0;
     QSqlQuery query;
     if (query.exec("SELECT SUM(VALUE) FROM BILL")) {
         if (query.next()) {
             total = query.value(0).toFloat();
             qDebug() << "Total bill amount:" << total;
         }
     }

     // Average bill amount
     int count = 0;
     float avg = 0;
     if (query.exec("SELECT COUNT(*), AVG(VALUE) FROM BILL")) {
         if (query.next()) {
             count = query.value(0).toInt();
             avg = query.value(1).toFloat();
             qDebug() << "Average bill amount:" << avg;
         }
     }

     // Maximum and minimum bill amounts
     float max = 0, min = 0;
     if (query.exec("SELECT MAX(VALUE), MIN(VALUE) FROM BILL")) {
         if (query.next()) {
             max = query.value(0).toFloat();
             min = query.value(1).toFloat();
             qDebug() << "Maximum bill amount:" << max;
             qDebug() << "Minimum bill amount:" << min;
         }
     }

     // Number of bills by insurance type
     QMap<QString, int> insuranceCounts;
     if (query.exec("SELECT INSURANCE, COUNT(*) FROM BILL GROUP BY INSURANCE")) {
         while (query.next()) {
             QString insurance = query.value(0).toString();
             int count = query.value(1).toInt();
             insuranceCounts[insurance] = count;
         }
     }
     qDebug() << "Number of bills by insurance type:" << insuranceCounts;

     // Average bill amount by insurance type
     QMap<QString, float> insuranceAverages;
     if (query.exec("SELECT INSURANCE, AVG(VALUE) FROM BILL GROUP BY INSURANCE")) {
         while (query.next()) {
             QString insurance = query.value(0).toString();
             float avg = query.value(1).toFloat();
             insuranceAverages[insurance] = avg;
         }
     }
     qDebug() << "Average bill amount by insurance type:" << insuranceAverages;




     QtCharts:: QBarSeries *barSeries = new QtCharts::QBarSeries();
     QtCharts::QChartView *barChartView = new QtCharts::QChartView();
     QtCharts::QChart *barChart = new QtCharts::QChart();
     barChart->setTitle("Number of bills by insurance type");

     // Create a bar set for each insurance type and add it to the bar series
     for (auto it = insuranceCounts.begin(); it != insuranceCounts.end(); ++it) {
         QtCharts::QBarSet *barSet = new QtCharts::QBarSet(it.key());
         *barSet << it.value();
         barSeries->append(barSet);
     }

     barChart->addSeries(barSeries);
     barChart->createDefaultAxes();
     QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();

     // Calculate the maximum count across all insurance types for Y axis range
     int maxCount = 0;
     for (auto it = insuranceCounts.begin(); it != insuranceCounts.end(); ++it) {
         if (it.value() > maxCount) {
             maxCount = it.value();
         }
     }
     axisY->setRange(0, maxCount);
     barChart->setAxisY(axisY, barSeries);

     // Disable the category axis
     QtCharts::QCategoryAxis *axisX = new QtCharts::QCategoryAxis();
     axisX->setVisible(false);
     barChart->setAxisX(axisX, barSeries);

     barChartView->setChart(barChart);
     barChartView->show();


     // Define a QMap to store the emergency type counts
     QMap<QString, int> emerTypeCounts;

     // Execute the SQL query to get the count of bills by emergency type
     if (query.exec("SELECT EMER_TYPE, COUNT(*) FROM BILL GROUP BY EMER_TYPE")) {
         while (query.next()) {
             QString emer_type = query.value(0).toString();
             int count = query.value(1).toInt();
             emerTypeCounts[emer_type] = count;
         }
     }

     // Create a QPieSeries object and add slices for each emergency type count
     QtCharts::QPieSeries *pieSeries = new QtCharts::QPieSeries();
     for (auto it = emerTypeCounts.begin(); it != emerTypeCounts.end(); ++it) {
         pieSeries->append(it.key(), it.value());
     }

     // Create a QChart object, add the pie series, and set the chart title
     QtCharts:: QChart *pieChart = new QtCharts::QChart();
     pieChart->setTitle("Number of bills by emergency type");
     pieChart->addSeries(pieSeries);

     // Create a QChartView object, set the chart, and display the pie chart
     QtCharts::QChartView *pieChartView = new QtCharts::QChartView();
     pieChartView->setChart(pieChart);
     pieChartView->show();

     if (QSslSocket::supportsSsl()) {
         qDebug() << "SSL/TLS is supported.";
     } else {
         qDebug() << "SSL/TLS is not supported.";
     }

}
