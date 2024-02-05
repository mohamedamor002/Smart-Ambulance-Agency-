#include"event.h"
#include <QFileDialog>
#include <QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QFileDialog>
#include <QPrinter>
#include <QPdfWriter>
#include <QPainter>
#include <QInputDialog>
#include <QByteArray>
#include <QBuffer>
#include <QFileInfo>
#include <QDebug>
#include <QSqlError>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMap>
#include <QtCharts>
Event::Event(){}
Event::Event(int eid, int cp, QString ct, QString cn, QString rl, QString l, QString r, QString pn,int pa, QString mc, QString cs, QString cdt, QString pr, QString pt, QString idt, int d, QString rd, int hid, int aid, int acid)
{
    this->EM_ID=eid;
    this->C_PHONE_NUMBER=cp;
    this->P_AGE=pa;
    this->DURATION=d;
    this->C_NAME=cn;
    this->C_TYPE=ct;
    this->RELATIONSHIP=rl;
    this->LOCATION=l;
    this->REASON=r;
    this->P_NAME=pn;
    this->MEDICAL_CHECKLIST=mc;
    this->CURRENT_STATE=cs;
    this->PATHOLOGY_RISKS=pr;
    this->PATHOLOGY_TYPES=pt;
    this->REGULATION_DECISION=rd;
    this->CALL_DT=cdt;
    this->INTERVENTION_DT=idt;
    this->H_ID=hid;
    this->A_ID=aid;
    this->AC_ID=acid;



}

Event::Event(const QSqlQuery &query)
    : EM_ID(query.value("EM_ID").toUInt()),
      C_PHONE_NUMBER(query.value("C_PHONE_NUMBER").toUInt()),
      P_AGE(query.value("P_AGE").toUInt()),
      DURATION(query.value("DURATION").toUInt()),
      H_ID(query.value("H_ID").toUInt()), A_ID(query.value("A_ID").toUInt()),
      AC_ID(query.value("AC_ID").toUInt()),
      C_NAME(query.value("C_NAME").toString()),
      C_TYPE(query.value("C_TYPE").toString()),
      RELATIONSHIP(query.value("RELATIONSHIP").toString()),
      LOCATION(query.value("LOCATION").toString()),
      REASON(query.value("REASON").toString()),
      P_NAME(query.value("P_NAME").toString()),
      MEDICAL_CHECKLIST(query.value("MEDICAL_CHECKLIST").toString()),
      CURRENT_STATE(query.value("CURRENT_STATE").toString()),
      CALL_DT(query.value("CALL_DT").toString()),
      PATHOLOGY_RISKS(query.value("PATHOLOGY_RISKS").toString()),
      PATHOLOGY_TYPES(query.value("PATHOLOGY_TYPES").toString()),
      INTERVENTION_DT(query.value("INTERVENTION_DT").toString()),
      REGULATION_DECISION(query.value("REGULATION_DECISION").toString()) {}

Event::Event(const QSqlRecord &record)
    : EM_ID(record.value("EM_ID").toUInt()),
      C_PHONE_NUMBER(record.value("C_PHONE_NUMBER").toUInt()),
      P_AGE(record.value("P_AGE").toUInt()),
      DURATION(record.value("DURATION").toUInt()),
      H_ID(record.value("H_ID").toUInt()), A_ID(record.value("A_ID").toUInt()),
      AC_ID(record.value("AC_ID").toUInt()),
      C_NAME(record.value("C_NAME").toString()),
      C_TYPE(record.value("C_TYPE").toString()),
      RELATIONSHIP(record.value("RELATIONSHIP").toString()),
      LOCATION(record.value("LOCATION").toString()),
      REASON(record.value("REASON").toString()),
      P_NAME(record.value("P_NAME").toString()),
      MEDICAL_CHECKLIST(record.value("MEDICAL_CHECKLIST").toString()),
      CURRENT_STATE(record.value("CURRENT_STATE").toString()),
      CALL_DT(record.value("CALL_DT").toString()),
      PATHOLOGY_RISKS(record.value("PATHOLOGY_RISKS").toString()),
      PATHOLOGY_TYPES(record.value("PATHOLOGY_TYPES").toString()),
      INTERVENTION_DT(record.value("INTERVENTION_DT").toString()),
      REGULATION_DECISION(record.value("REGULATION_DECISION").toString()) {}

bool Event::Create() const
{
    QSqlQuery  query;

    // prepare() takes the query as a parameter to prepare it for execution
    query.prepare(
        "INSERT INTO EMERGENCY"
        "("
        "EM_ID, C_PHONE_NUMBER, C_TYPE ,C_NAME, "
        "RELATIONSHIP, LOCATION, REASON, P_NAME, P_AGE, MEDICAL_CHECKLIST, "
        "CURRENT_STATE, CALL_DT, PATHOLOGY_RISKS, PATHOLOGY_TYPES, "
        "INTERVENTION_DT, DURATION, REGULATION_DECISION,"
        " H_ID,"
        " A_ID,"
        " AC_ID"
        " )"
        "VALUES"
        "("
        ":EM_ID, :C_PHONE_NUMBER, :C_TYPE, :C_NAME, :RELATIONSHIP, "
        ":LOCATION, :REASON,  :P_NAME, :P_AGE, :MEDICAL_CHECKLIST, "
        ":CURRENT_STATE, :CALL_DT, :PATHOLOGY_RISKS, :PATHOLOGY_TYPES, "
        ":INTERVENTION_DT, :DURATION, :REGULATION_DECISION,"
        ":H_ID,"
        ":A_ID,"
        ":AC_ID"
        ")");

    // Creating variables
    query.bindValue(":EM_ID", EM_ID);
    query.bindValue(":C_PHONE_NUMBER", C_PHONE_NUMBER);
    query.bindValue(":P_AGE", P_AGE);
    query.bindValue(":DURATION", DURATION);
    query.bindValue(":H_ID", H_ID);
    query.bindValue(":A_ID", A_ID);
    query.bindValue(":C_NAME", C_NAME);
    query.bindValue(":C_TYPE", C_TYPE);
    query.bindValue(":RELATIONSHIP", RELATIONSHIP);
    query.bindValue(":LOCATION", LOCATION);
    query.bindValue(":REASON", REASON);
    query.bindValue(":P_NAME", P_NAME);
    query.bindValue(":MEDICAL_CHECKLIST", MEDICAL_CHECKLIST);
    query.bindValue(":CURRENT_STATE", CURRENT_STATE);
    query.bindValue(":PATHOLOGY_RISKS", PATHOLOGY_RISKS);
    query.bindValue(":PATHOLOGY_TYPES", PATHOLOGY_TYPES);
    query.bindValue(":REGULATION_DECISION", REGULATION_DECISION);
    query.bindValue(":CALL_DT", CALL_DT);
    query.bindValue(":INTERVENTION_DT", INTERVENTION_DT);
    query.bindValue(":AC_ID", AC_ID);

    return query.exec(); //exec() send request to be executed
}

QSqlQuery Event::Read(const QString &searchQuery, Column searchAndQueryColumn,
                      Status status,
                          FetchMode fetchMode) {
    // QSqlQueryModel * model=new QSqlQueryModel();

    QSqlQuery query;

    // Additional query for filtering by status
    const QMap<Status, QString> statusToQuery{
        {Status::Any, QString()},
        {Status::Active, " AND INTERVENTION_DT IS NULL"},
        {Status::Inactive, " AND INTERVENTION_DT IS NOT NULL"},
    };

    // Which column to search and/or order by
    const QMap<Column, QString> targetColumnName{
        {Column::PatientName, "P_NAME"},
        {Column::Id, "EM_ID"},
        {Column::CallDate, "CALL_DT"},
    };

    // '1 > 0' is there to allow concatenating with an existing WHERE clause
    // with no issue
    const QString queryString =
        QString("SELECT %1 from emergency e WHERE 1 > 0")
            .arg(fetchMode == FetchMode::All ? "e.*" : "'[' || e.EM_ID || '] ' || e.LOCATION FORMATTEDNAME, e.EM_ID") +
        (!searchQuery.isEmpty()
             ? QString(" AND %1 LIKE '%' || ? || '%'")
                   .arg(targetColumnName[searchAndQueryColumn])
             : QString()) +
        statusToQuery[status] + " ORDER BY " +
        targetColumnName[searchAndQueryColumn];

    query.prepare(queryString); // When empty this causes no issues

    if (!searchQuery.isEmpty()) {
      query.addBindValue(searchQuery);
    }

    query.exec();

    // model->setQuery(query);

    // model->setHeaderData(0,Qt::Horizontal,QObject::tr("EM_ID"));
    // model->setHeaderData(1,Qt::Horizontal,QObject::tr("C_PHONE_NUMBER"));
    // model->setHeaderData(2,Qt::Horizontal,QObject::tr("C_TYPE"));
    // model->setHeaderData(3,Qt::Horizontal,QObject::tr("C_NAME"));
    // model->setHeaderData(4,Qt::Horizontal,QObject::tr("RELATIONSHIP"));
    // model->setHeaderData(5,Qt::Horizontal,QObject::tr("LOCATION"));
    // model->setHeaderData(6,Qt::Horizontal,QObject::tr("REASON"));
    // model->setHeaderData(7,Qt::Horizontal,QObject::tr("P_NAME"));
    // model->setHeaderData(8,Qt::Horizontal,QObject::tr("P_AGE"));
    // model->setHeaderData(9,Qt::Horizontal,QObject::tr("MEDICAL_CHECKLIST"));
    // model->setHeaderData(10,Qt::Horizontal,QObject::tr("CURRENT_STATE"));
    // model->setHeaderData(11,Qt::Horizontal,QObject::tr("CALL_DT"));
    // model->setHeaderData(12,Qt::Horizontal,QObject::tr("PATHOLOGY_RISKS"));
    // model->setHeaderData(13,Qt::Horizontal,QObject::tr("PATHOLOGY_TYPES"));
    // model->setHeaderData(14,Qt::Horizontal,QObject::tr("INTERVENTION_DT"));
    // model->setHeaderData(15,Qt::Horizontal,QObject::tr("DURATION"));
    // model->setHeaderData(16,Qt::Horizontal,QObject::tr("REGULATION_DECISION"));
    // model->setHeaderData(17,Qt::Horizontal,QObject::tr("H_ID"));
    // model->setHeaderData(18,Qt::Horizontal,QObject::tr("A_ID"));
    // model->setHeaderData(19,Qt::Horizontal,QObject::tr("AC_ID"));

    return query;
}

QSqlQuery Event::Read(Status status,
                          FetchMode fetchMode) {
    return Read(QString(), Column::Id, status, fetchMode);
}

bool Event::Update() const
{

    QSqlQuery qry;

    qry.prepare("UPDATE emergency SET "
                "C_PHONE_NUMBER = :C_PHONE_NUMBER, P_AGE = :P_AGE, "
                "DURATION = :DURATION, "
                "H_ID = :H_ID, "
                "A_ID = :A_ID, "
                "C_NAME = :C_NAME, "
                "C_TYPE = :C_TYPE, "
                "RELATIONSHIP = :RELATIONSHIP, "
                "LOCATION = :LOCATION, "
                "REASON = :REASON, P_NAME=:P_NAME, "
                "MEDICAL_CHECKLIST = :MEDICAL_CHECKLIST, "
                "CURRENT_STATE = :CURRENT_STATE, "
                "PATHOLOGY_RISKS = :PATHOLOGY_RISKS, "
                "PATHOLOGY_TYPES = :PATHOLOGY_TYPES, "
                "REGULATION_DECISION = :REGULATION_DECISION, "
                "CALL_DT = :CALL_DT, INTERVENTION_DT = :INTERVENTION_DT, "
                "AC_ID = :AC_ID "
                "WHERE EM_ID = :EM_ID");

    qry.bindValue(":EM_ID", EM_ID);
    qry.bindValue(":C_PHONE_NUMBER",C_PHONE_NUMBER);
    qry.bindValue(":P_AGE", P_AGE);
    qry.bindValue(":DURATION", DURATION);
    qry.bindValue(":H_ID", H_ID);
    qry.bindValue(":A_ID", AC_ID);
    qry.bindValue(":C_NAME", C_NAME);
    qry.bindValue(":C_TYPE", C_TYPE);
    qry.bindValue(":RELATIONSHIP", RELATIONSHIP);
    qry.bindValue(":LOCATION", LOCATION);
    qry.bindValue(":REASON", REASON);
    qry.bindValue(":P_NAME", P_NAME);
    qry.bindValue(":MEDICAL_CHECKLIST", MEDICAL_CHECKLIST);
    qry.bindValue(":CURRENT_STATE", CURRENT_STATE);
    qry.bindValue(":PATHOLOGY_RISKS", PATHOLOGY_RISKS);
    qry.bindValue(":PATHOLOGY_TYPES", PATHOLOGY_TYPES);
    qry.bindValue(":REGULATION_DECISION", REGULATION_DECISION);
    qry.bindValue(":CALL_DT", CALL_DT);
    qry.bindValue(":INTERVENTION_DT", INTERVENTION_DT);
    qry.bindValue(":AC_ID", AC_ID);


    return qry.exec();
}

bool Event::Delete(int EM_ID)
{
    QSqlQuery query;
    QString res=QString::number(EM_ID);

    query.prepare("Delete from emergency where EM_ID= :EM_ID");

    query.bindValue(":EM_ID",res);

    return  query.exec();






}

// QSqlQueryModel *Event::sortid(QString o)
// {

//       if (o == "id") {
//           o = "EM_ID";
//       }
//        else if (o == "name") {
//           o = "P_NAME";
//       } else if (o == "P_AGE") {
//           o = "P_AGE";
//       } else if (o == "MEDICAL_CHECKLIST") {
//           o = "MEDICAL_CHECKLIST";
//       } else if (o == "CURRENT_STATE") {
//           o = "CURRENT_STATE";
//       } else if (o == "CALL_DT") {
//           o = "CALL_DT";
//       } else if (o == "PATHOLOGY_RISKS") {
//           o = "PATHOLOGY_RISKS";
//       } else if (o == "PATHOLOGY_TYPES") {
//           o = "PATHOLOGY_TYPES";
//       } else if (o == "INTERVENTION_DT") {
//           o = "INTERVENTION_DT";
//       } else if (o == "DURATION") {
//           o = "DURATION";
//       }
//       QSqlQueryModel* model = new QSqlQueryModel();
//       model->setQuery("SELECT * FROM emergency ORDER BY " + o);
//       model->setHeaderData(0, Qt::Horizontal, QObject::tr("EM_ID"));
//       model->setHeaderData(1, Qt::Horizontal, QObject::tr("C_PHONE_NUMBER"));
//       model->setHeaderData(2, Qt::Horizontal, QObject::tr("C_TYPE"));
//       model->setHeaderData(3, Qt::Horizontal, QObject::tr("C_NAME"));
//       model->setHeaderData(4, Qt::Horizontal, QObject::tr("RELATIONSHIP"));
//       model->setHeaderData(5, Qt::Horizontal, QObject::tr("LOCATION"));
//       model->setHeaderData(6, Qt::Horizontal, QObject::tr("REASON"));
//       model->setHeaderData(7, Qt::Horizontal, QObject::tr("P_NAME"));
//       model->setHeaderData(8, Qt::Horizontal, QObject::tr("P_AGE"));
//       model->setHeaderData(9, Qt::Horizontal, QObject::tr("MEDICAL_CHECKLIST"));
//       model->setHeaderData(10, Qt::Horizontal, QObject::tr("CURRENT_STATE"));
//       model->setHeaderData(11, Qt::Horizontal, QObject::tr("CALL_DT"));
//       model->setHeaderData(12, Qt::Horizontal, QObject::tr("PATHOLOGY_RISKS"));
//       model->setHeaderData(13, Qt::Horizontal, QObject::tr("PATHOLOGY_TYPES"));


//     return  model;
// }





void Event :: exportEmergency(int id,QString fileName)
{

    // Execute a query to retrieve the emergency information by ID
    QSqlQuery query;
    query.prepare("SELECT * FROM emergency WHERE EM_ID = ?");
    query.addBindValue(id);

    if (!query.exec())
    {
      qDebug() << "Failed to execute query: " << query.lastError().text();
      return;
    }

    // If the query returned a result, save the hospital information to a PDF file
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
          painter.setPen(QColor("#0072C6")); // Use a blue color

          // Draw the hospital information onto the painter
          // int EM_ID = query.value(0).toInt();
          //int C_PHONE_NUMBER = query.value(1).toInt();
          //int DURATION = query.value(2).toInt();
          //int P_AGE = query.value(3).toInt();
          // int H_ID = query.value(4).toInt();
          // int A_ID = query.value(5).toInt();
          // int AC_ID = query.value(6).toInt();
          QString C_NAME = query.value(7).toString();
          QString C_TYPE = query.value(8).toString();
          QString RELATIONSHIP = query.value(9).toString();
          QString LOCATION = query.value(10).toString();
          QString REASON = query.value(11).toString();
          QString P_NAME = query.value(12).toString();
          QString MEDICAL_CHECKLIST = query.value(13).toString();
          QString CURRENT_STATE = query.value(14).toString();
          QString PATHOLOGY_RISKS = query.value(15).toString();
          QString PATHOLOGY_TYPES = query.value(16).toString();
          QString REGULATION_DECISION = query.value(17).toString();
          QString CALL_DT = query.value(18).toString();
          QString INTERVENTION_DT = query.value(19).toString();


          painter.drawText(3000, 700, "Emergency Information");

          int x = 500;
          int y = 2000;

          painter.setFont(normalFont);
          painter.setPen(Qt::black);

          painter.drawText(x, y, "Emergency ID:");
          painter.setPen(QColor("#0072C6"));
          // painter.drawText(x + 1600, y, EM_ID);
          painter.setPen(Qt::black);

          y += 350;
          painter.drawText(x, y, "Caller phone number:");
          painter.setPen(QColor("#0072C6"));
          // painter.drawText(x + 1600, y, C_PHONE_NUMBER);
          painter.setPen(Qt::black);

          y += 350;
          painter.drawText(x, y, "Duration:");
          painter.setPen(QColor("#0072C6"));
          //painter.drawText(x + 1600, y, DURATION);
          painter.setPen(Qt::black);

          y += 350;

          painter.drawText(x, y, "Patient age:");
          painter.setPen(QColor("#0072C6"));
          //painter.drawText(x + 1600, y, P_AGE);
          painter.setPen(Qt::black);

          y += 350;


          painter.drawText(x, y, "Hospital ID:");
          painter.setPen(QColor("#0072C6"));
          //painter.drawText(x + 1600, y, H_ID);
          painter.setPen(Qt::black);


          y += 350;

          painter.drawText(x, y, "Ambulance ID:");
          painter.setPen(QColor("#0072C6"));
          //painter.drawText(x + 1600, y, A_ID);
          painter.setPen(Qt::black);


          y += 350;

          painter.drawText(x+5000, y, "Account ID:");
          painter.setPen(QColor("#0072C6"));
          //painter.drawText(x + 1600, y, AC_ID);
          painter.setPen(Qt::black);

          y += 350;
          painter.drawText(x, y, "Caller Name:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, C_NAME);
          painter.setPen(Qt::black);

          y += 350;
          painter.drawText(x, y, "Caller type:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, C_TYPE);
          painter.setPen(Qt::black);

          y += 350;
          painter.drawText(x, y, "Relationship:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, RELATIONSHIP);
          painter.setPen(Qt::black);

          y += 350;
          painter.drawText(x, y, "Location:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, LOCATION);
          painter.setPen(Qt::black);
          y += 350;

          painter.drawText(x, y, "Reason:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, REASON);
          painter.setPen(Qt::black);


          y += 350;

          painter.drawText(x+5000, y, "Patient name:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, P_NAME);
          painter.setPen(Qt::black);

          y += 350;

          painter.drawText(x, y, "Medical checklist:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, MEDICAL_CHECKLIST);
          painter.setPen(Qt::black);

          y += 350;

          painter.drawText(x, y, "Current state:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, CURRENT_STATE);
          painter.setPen(Qt::black);


          y += 350;

          painter.drawText(x+5000, y, "Pathology risks:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, PATHOLOGY_RISKS);
          painter.setPen(Qt::black);


          y += 350;

          painter.drawText(x, y, "¨Pathology types:");
              painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, PATHOLOGY_TYPES);
          painter.setPen(Qt::black);

          y += 350;

          painter.drawText(x, y, "Regulation decision:");
          painter.setPen(QColor("#0072C6"));
          painter.drawText(x + 1600, y, REGULATION_DECISION);
          painter.setPen(Qt::black);


          y += 350;

          painter.drawText(x+5000, y, "Call date:");
          painter.setPen(QColor("#0072C6"));
          //painter.drawText(x + 1600, y, CALL_DT);
          painter.setPen(Qt::black);

          y += 350;

          painter.drawText(x+5000, y, "Intervention date:");
          painter.setPen(QColor("#0072C6"));
          //painter.drawText(x + 1600, y, INTERVENTION_DT);
          painter.setPen(Qt::black);


          painter.end();

      }
    }



}

void Event::statEmergency()
{
    QSqlQuery query;
    // Number of emergencies by reason

    QMap<QString, int> reasonCounts;

    // Execute the SQL query to get the count of emergencies by reason
    if (query.exec("SELECT reason, COUNT(*) FROM emergency GROUP BY reason")) {
      while (query.next()) {
          QString reason = query.value(0).toString();
          int count = query.value(1).toInt();
          reasonCounts[reason] = count;
      }
    }

    // Create a bar series and chart to display the results
    QtCharts::QBarSeries *barSeries = new QtCharts::QBarSeries();
    QtCharts::QChartView *barChartView = new QtCharts::QChartView();
    QtCharts::QChart *barChart = new QtCharts::QChart();
    barChart->setTitle("Number of emergencies by reason");

    // Create a bar set for each reason and add it to the bar series
    for (auto it = reasonCounts.begin(); it != reasonCounts.end(); ++it) {
      QtCharts::QBarSet *barSet = new QtCharts::QBarSet(it.key());
      *barSet << it.value();
      barSeries->append(barSet);
    }

    barChart->addSeries(barSeries);
    barChart->createDefaultAxes();
    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();

    // Calculate the maximum count across all reasons for Y axis range
    int maxCount = 0;
    for (auto it = reasonCounts.begin(); it != reasonCounts.end(); ++it) {
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
    barChartView->show() ;

}
