#include "hospital.h"
hospital:: hospital(){}
hospital:: hospital(QString name, QString adress,QString speciality ,QString type,QString email,QString s_id,int phone, int nb_beds,int nb_patients,int  id )
{

this->id=id ;
this->name=name ;
this -> adress=adress ;
this->speciality =speciality ;
this->type=type ;
this->email=email ;
this-> phone=phone ;
this ->nb_beds=nb_beds ;
this->nb_patients=nb_patients ;
this->s_id=s_id ;

}
bool hospital:: create()
{
    QSqlQuery  query;

   QString id1 = QString::number(id);
   QString phone1 = QString::number(phone);
   QString nb_beds1 = QString::number(nb_beds);
   QString nb_patients1= QString::number(nb_patients);

    //prepare() takes the query as a parameter to prepare it for execution
    query.prepare("insert into hospital (H_ID, PHONE,S_ID,NBR_BEDS,NBR_PATIENTS,ADDRESS,TYPE,HNAME,SPECIALITY,EMAIL) values (:H_ID, :PHONE,:S_ID,:NBR_BEDS,:NBR_PATIENTS,:ADDRESS,:TYPE,:HNAME,:SPECIALITY,:EMAIL)");

    // Creating variables
    query.bindValue(":H_ID",id1);
    query.bindValue(":PHONE",phone1);
    query.bindValue(":S_ID",s_id);
    query.bindValue(":NBR_BEDS",nb_beds1);
    query.bindValue(":NBR_PATIENTS",nb_patients1);
    query.bindValue(":ADDRESS",adress);
    query.bindValue(":TYPE",type);
    query.bindValue(":HNAME",name);
    query.bindValue(":SPECIALITY",speciality);
    query.bindValue(":EMAIL",email);

    return query.exec();
}

bool hospital::Delete(int id)
{
    QSqlQuery query;
    QString res=QString::number(id);
    query.prepare("Delete from hospital where H_ID= :id");

    query.bindValue(":id",res);

    return  query.exec();

}

QSqlQueryModel * hospital::read()
{
    QSqlQueryModel * model=new QSqlQueryModel();

    model->setQuery("select * from hospital");
    model->setHeaderData(0,Qt::Horizontal,QObject::tr("H_ID"));
    model->setHeaderData(1,Qt::Horizontal,QObject::tr("HNAME"));
    model->setHeaderData(2,Qt::Horizontal,QObject::tr("ADRESS"));
    model->setHeaderData(3,Qt::Horizontal,QObject::tr("PHONE"));
    model->setHeaderData(4,Qt::Horizontal,QObject::tr("EMAIL"));
    model->setHeaderData(5,Qt::Horizontal,QObject::tr("SPECIALITY"));
    model->setHeaderData(6,Qt::Horizontal,QObject::tr("TYPE"));
    model->setHeaderData(7,Qt::Horizontal,QObject::tr("NBR_BEDS"));
    model->setHeaderData(8,Qt::Horizontal,QObject::tr("NBR_PATIENTS"));
    model->setHeaderData(9,Qt::Horizontal,QObject::tr("S_ID"));

    return  model;
}

bool hospital::Update()
{


   QString id1 = QString::number(id);
   QString phone1 = QString::number(phone);
   QString nb_beds1 = QString::number(nb_beds);
   QString nb_patients1= QString::number(nb_patients);
   QSqlQuery  query;
    //prepare() takes the query as a parameter to prepare it for execution
   query.prepare("UPDATE hospital SET PHONE=:phone, S_ID=:s_id, NBR_BEDS=:nb_beds, NBR_PATIENTS=:nb_patients, ADDRESS=:adress, TYPE=:type, HNAME=:name, SPECIALITY=:speciality, EMAIL=:email  WHERE H_ID=:id ");




    // Creating variables
    query.bindValue(":id",id);
    query.bindValue(":phone",phone1);
    query.bindValue(":s_id",s_id);
    query.bindValue(":nb_beds",nb_beds1);
    query.bindValue(":nb_patients",nb_patients1);
    query.bindValue(":adress",adress);
    query.bindValue(":type",type);
    query.bindValue(":name",name);
    query.bindValue(":speciality",speciality);
    query.bindValue(":email",email);
    return query.exec();


}


QSqlQueryModel * hospital ::search(QString s)
{     QSqlQuery  query;
    query.prepare("SELECT * FROM hospital WHERE HNAME LIKE '%" + s + "%' OR H_ID LIKE '%" + s + "%'");
    query.exec() ;


    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);



    return model ;


}


QSqlQueryModel * hospital ::sort(QString o)
{



    if(o=="id") o="H_ID" ;
    else if (o=="name") o="HNAME"  ;
    else if (o=="nb_beds") o="NBR_BEDS"  ;
    QSqlQueryModel * model=new QSqlQueryModel();

    model->setQuery("select * from hospital ORDER BY "+ o);
    model->setHeaderData(0,Qt::Horizontal,QObject::tr("H_ID"));
    model->setHeaderData(1,Qt::Horizontal,QObject::tr("HNAME"));
    model->setHeaderData(2,Qt::Horizontal,QObject::tr("ADRESS"));
    model->setHeaderData(3,Qt::Horizontal,QObject::tr("PHONE"));
    model->setHeaderData(4,Qt::Horizontal,QObject::tr("EMAIL"));
    model->setHeaderData(5,Qt::Horizontal,QObject::tr("SPECIALITY"));
    model->setHeaderData(6,Qt::Horizontal,QObject::tr("TYPE"));
    model->setHeaderData(7,Qt::Horizontal,QObject::tr("NBR_BEDS"));
    model->setHeaderData(8,Qt::Horizontal,QObject::tr("NBR_PATIENTS"));
    model->setHeaderData(9,Qt::Horizontal,QObject::tr("S_ID"));

    return  model;


}
void hospital ::exportHospital(int id , QString fileName)
{
    // Execute a query to retrieve the hospital information by ID
    QSqlQuery query;
    query.prepare("SELECT * FROM hospital WHERE H_ID = ?");
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
            QString name = query.value("HNAME").toString();
            QString address = query.value("ADDRESS").toString();
            QString email = query.value("EMAIL").toString();
            QString phone = query.value("PHONE").toString();
            QString speciality = query.value("SPECIALITY").toString();
            QString services = query.value("S_ID").toString();

            painter.drawText(3000, 700, "Hospital Information");

            int x = 500;
            int y = 2000;

            painter.setFont(normalFont);
            painter.setPen(Qt::black);

            painter.drawText(x, y, "Name:");
            painter.setPen(QColor("#0072C6"));
            painter.drawText(x + 1600, y, name);
            painter.setPen(Qt::black);

            y += 350;
            painter.drawText(x, y, "Address:");
            painter.setPen(QColor("#0072C6"));
            painter.drawText(x + 1600, y, address);
            painter.setPen(Qt::black);

            y += 350;
            painter.drawText(x, y, "Email:");
            painter.setPen(QColor("#0072C6"));
            painter.drawText(x + 1600, y, email);
            painter.setPen(Qt::black);

            y += 350;
            painter.drawText(x, y, "Phone:");
            painter.setPen(QColor("#0072C6"));
            painter.drawText(x + 1600, y, phone);
            painter.setPen(Qt::black);

            y += 350;
            painter.drawText(x, y, "Speciality:");
            painter.setPen(QColor("#0072C6"));
            painter.drawText(x + 1600, y, speciality);
            painter.setPen(Qt::black);

            y += 350;
            painter.drawText(x, y, "Services:");
            painter.setPen(QColor("#0072C6"));
            painter.drawText(x + 1600, y, services);
            painter.setPen(Qt::black);

            painter.end();

        }
    }

}
