#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ambulance.h"
#include "arduino.h"
#include "audiotranscript.h"
#include "bill.h"
#include "calendar.h"
#include "dialog.h"
#include "equipment.h"
#include "event.h"
#include "hospital.h"
#include "staff.h"
#include "staffwidget.h"
#include <QButtonGroup>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QSqlRecord>
#include <QTableView>
#include <QtGlobal>
#include <QtWidgets>
#include <QGeoPositionInfoSource>
#include <QGeoLocation>
#include <QGeoAddress>
#include <algorithm>
#include <map>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr, const Staff &member = {});
  ~MainWindow();
signals:
  void coordsMadeAvailable(const QGeoPositionInfo &info);
  void locationMadeAvailable(const QString &loc);

private slots:


    bool on_lineEditManufacEquip_editingFinished();

    void on_comboBoxFilterEquip_activated(const QString &arg1);

    void on_lineEditSearchEquip_textChanged(const QString &arg1);

    void search(int id, QTableView* tableView, QLineEdit* idLineEdit);

    void on_pushButtonExportAmbBtn_clicked();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void highlightStartDates();

    void on_pushButtonSearchAmbbtn_clicked();

    void on_tableView_activated(const QModelIndex &index);



    void on_pushButtonStatAmbBtn_clicked();

private:
    Ui::MainWindow *ui;
    Equipment Etmp ;
    ambulance Etmp1;
    hospital hos ;
    calendar cl;
    bill Bi;
    Event Ev ;
    Staff user;

    // Staff tools

    StaffWidget * staffW = nullptr;

    // Emergency tools

    QButtonGroup * emergencyInterfaceButtonGroup;
    // QButtonGroup * emergencyRegulationRadioGroup;
    QSqlQueryModel * emergenciesModel;
    Event eventFromForm() ;
    void loadTableEventOnForm() ;

    void setWindowInterface() ;
    void  setEquipmentInterface() ;
    void setWindowAmbulance() ;
    void setWindowEmergency() ;
    void setWindowHospital() ;
    void setWindowBill() ;
    void setwindowHome() ;
    void  Equipment_CRUD() ;
    void Ambulance_CRUD() ;
    void Calendar_CRUD();
    void Bill_CRUD() ;

    void staffSetup();
    void getMapImage() ;

    void Hospital_CRUD() ;
    Equipment GetEquipmentFromForm() ;
     QString FilterEquip;
     QString SearchEquip;
     QByteArray dataTraffic; // variable containing the received data
     qint64  lastScanTime = QDateTime::currentSecsSinceEpoch() -1 ;

     // Arduino
     void arduinoSetup() ;

     // Transcript
     
     void transcriptSetup();

     // Weather

     QString weatherLocation = QString();
};
#endif // MAINWINDOW_H
