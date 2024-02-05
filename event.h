#ifndef EVENT_H
#define EVENT_H
#include <QSqlQuery>
#include <QString>
#include <QSqlRecord>
#include <QSqlQueryModel>

class Event
{
    int EM_ID, C_PHONE_NUMBER, P_AGE,DURATION,H_ID, A_ID, AC_ID;
    QString C_NAME, C_TYPE, RELATIONSHIP, LOCATION, REASON, P_NAME,MEDICAL_CHECKLIST, CURRENT_STATE,
        CALL_DT, PATHOLOGY_RISKS, PATHOLOGY_TYPES, INTERVENTION_DT,REGULATION_DECISION;





public:

    Event();
    Event(int eid, int cp, QString ct, QString cn, QString rl, QString l,
          QString r, QString pn, int pa, QString mc, QString cs, QString cdt,
          QString pr, QString pt, QString idt, int d, QString rd, int hid,
          int aid, int acid);
    Event(const QSqlQuery &query);
    Event(const QSqlRecord &record);

    //Getters
    int getEMID()const {return  EM_ID;}
    int getcphonenumber()const {return  C_PHONE_NUMBER;}
    QString getCType()const {return  C_TYPE;}
    QString getCName()const {return  C_NAME;}
    QString getRelationship()const {return  RELATIONSHIP;}
    QString getLocation()const {return  LOCATION;}
    QString getreason()const {return  REASON;}
    QString getpName()const {return  P_NAME;}
    int getPAge()const {return  P_AGE;}
    QString getChecklist()const {return  MEDICAL_CHECKLIST;}
    QString getCurrentstate()const {return  CURRENT_STATE;}
    QString getCDate()const {return  CALL_DT;}
    QString getCpathologyrisks()const {return  PATHOLOGY_RISKS;}
    QString getpathologytypes()const {return  PATHOLOGY_TYPES;}
    QString getIDate()const {return  INTERVENTION_DT;}
    int getduration()const {return  DURATION;}
    QString getregulationdecision()const {return  REGULATION_DECISION;}
    int gethid()const {return  H_ID;}
    int getaid()const {return  A_ID;}
    int getacid()const {return  AC_ID;}




    //Setters
    void setEMID(int eid){this->EM_ID=eid;}
    void setcphonenumber(int CP){this->C_PHONE_NUMBER=CP;}
    void setCType(QString ct){C_TYPE=ct;}
    void setCName(QString cn){C_NAME=cn;}
    void setRelationship(QString rl){RELATIONSHIP=rl;}
    void setCLocation(QString l){LOCATION=l;}
    void setReason(QString r){REASON=r;}
    void setPName(QString pn){P_NAME=pn;}
    void setPAge(int pa){this->P_AGE=pa;}
    void setChecklist(QString mc){MEDICAL_CHECKLIST=mc;}
    void setState(QString cs){CURRENT_STATE=cs;}
    void setCDate(QString cdt){CALL_DT=cdt;}
    void setRisks(QString pr){PATHOLOGY_RISKS=pr;}
    void setTypes(QString pt){PATHOLOGY_TYPES=pt;}
    void setIDate(QString idt){INTERVENTION_DT=idt;}
    void setduration(int d){this->DURATION=d;}
    void setregulationdecision(QString rd){REGULATION_DECISION=rd;}
    void sethid(int hid){this->H_ID=hid;}
    void setaid(int aid){this->A_ID=aid;}
    void setacid(int acid){this->AC_ID=acid;}



    // for searching
    enum class Status { Active, Inactive, Any };
    enum class Column { Id, PatientName, CallDate };
    enum class FetchMode { All, Formatted };

    // event base features
    bool Create () const;
    static QSqlQuery Read(const QString &searchQuery = QString(),
                          Column searchAndQueryColumn = Column::Id,
                          Status status = Status::Any,
                          FetchMode fetchMode = FetchMode::All);
    // static QSqlQuery ReadFormatted(const QString &searchQuery = QString(),
    //                       Column searchAndQueryColumn = Column::Id,
    //                       Status status = Status::Any);
    static QSqlQuery Read(Status status = Status::Any,
                          FetchMode fetchMode = FetchMode::All);
    // static QSqlQueryModel * sortid(QString o);
    static bool Delete (int id);
    static QSqlQuery generalInfo (int id);
    bool Update() const;

    //event specific features
    bool search();
    bool sort();
    bool exportToPdf();
    static  void exportEmergency(int id,QString fileName) ;
     static void statEmergency() ;



};

#endif
    // EVENT_H
