#include "mainwindow.h"
#include "assistant.h"
#include "ui_mainwindow.h"
#include "weather.h"
MainWindow::MainWindow(QWidget *parent, const Staff &loggedInuser)
    : QMainWindow(parent), ui(new Ui::MainWindow), user(loggedInuser) {
  ui->setupUi(this);
  connect(ui->pushButton, &QPushButton::clicked, this,
          &MainWindow::getMapImage);

  // Setting Window Functionalities
  setWindowInterface();

  // setting window

  setwindowHome();

  // Hide all by default

  foreach (QPushButton *wBut, ui->frame_2->findChildren<QPushButton *>(
                                  QString(), Qt::FindDirectChildrenOnly)) {
    if (wBut == ui->HomeBtn)
      continue;
    wBut->hide();
  }

  // Setting Equipment tab

  if (user.getRole() == "Administrator") {
    setEquipmentInterface();
    Equipment_CRUD();
    ui->EquipBtn->show();
  }
  // Emergency
  if (user.getRole() == "Operator") {
    setWindowEmergency();
    ui->EmergencyBtn->show();
    ui->TranscriptBtn->show();
    transcriptSetup();
    ui->RightMenuContainer->show();

    QSqlQueryModel *activeEmergenciesModel =
        new QSqlQueryModel(ui->activeEmergenciesList);

    // Get the active emergencies in a single formatted column

    activeEmergenciesModel->setQuery(
        Event::Read(Event::Status::Active, Event::FetchMode::Formatted));

    // set the model accordingly

    ui->activeEmergenciesList->setModel(activeEmergenciesModel);

    ui->activeEmergenciesList->setSelectionBehavior(
        QAbstractItemView::SelectRows);

    // Refresh every 2 seconds
    QTimer *timer = new QTimer(ui->activeEmergenciesList);
    connect(timer, &QTimer::timeout, ui->activeEmergenciesList,
            [activeEmergenciesModel]() {
              // refresh
              activeEmergenciesModel->setQuery(Event::Read(
                  Event::Status::Active, Event::FetchMode::Formatted));
            });
    timer->start(2000);

    // On double click, send to emergencies page
    connect(
        ui->activeEmergenciesList, &QListView::activated, this,
        [this](const QModelIndex &index) {
          if (!index.isValid())
            return;

          // activate the emergency page
          ui->EmergencyBtn->click();

          // activate the emergency page
          ui->emergencyStackedWidget->setCurrentWidget(ui->emergencyListPage);
          ui->emergencyListButton->click();

          // get the id

          const auto emId = dynamic_cast<const QSqlQueryModel *>(index.model())
                                ->query()
                                .value("EM_ID")
                                .toUInt();

          // Find  in the emergency table and click

          const auto list = emergenciesModel->match(
              emergenciesModel->index(
                  0, emergenciesModel->record().indexOf("EM_ID")),
              Qt::DisplayRole, emId, 1, Qt::MatchExactly);

          // Not found
          if (list.isEmpty())
            return;

          // Set current one
          ui->emergenciesTableView->setCurrentIndex(list.constFirst());
        });

  } else {
    ui->RightMenuContainer->hide();
  }

  // Ambulance
  if (user.getRole() == "Paramedic" || user.getRole() == "Operator") {
    setWindowAmbulance();
    Ambulance_CRUD();
    ui->tableView->setModel(Etmp1.Read());
    /*connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateTableView(QString)));*/
    // Calendar
    Calendar_CRUD();
    ui->RdvlistView->setModel(cl.display());
    highlightStartDates();
    // Find the QCalendarWidget object
    ui->AmbulanceBtn->show();
  }

  if (user.getRole() == "Administrator" || user.getRole() == "Operator") {
    // HOSPITAL
    Hospital_CRUD();
    setWindowHospital();
    ui->HospitalBtn->show();
    // Bill
    Bill_CRUD();
    setWindowBill();
    ui->BillBtn->show();
  }

  // Staff

  if (user.getRole() == "Administrator") {
    staffSetup();
    staffW->loadMembersList();
    ui->StaffBtn->show();
    ui->SmartBtn->show();
  }

  arduinoSetup();
}

MainWindow::~MainWindow() { delete ui; }
void MainWindow::setEquipmentInterface() {

  ui->comboBoxStatusEquip->addItem("Expired");
  ui->comboBoxStatusEquip->addItem("Valid");

  ui->comboBoxNameEquip->addItem(QIcon(":/equipment/img/téléchargement.jpg"),
                                 "Oxygen Tank");
  ui->comboBoxNameEquip->addItem(
      QIcon(":/equipment/img/when-was-the-defibrillator-invented.jpg"),
      "Defibrillator");
  ui->comboBoxNameEquip->addItem(
      QIcon(":/equipment/img/téléchargement (1).jpg"), "Heart monitor");
  ui->comboBoxNameEquip->addItem(
      QIcon(":/equipment/img/"
            "png-clipart-intravenous-therapy-infusion-set-dialysis-catheter-"
            "syringe-catheter-miscellaneous-blue-thumbnail.png"),
      "IV");
  ui->comboBoxNameEquip->addItem(QIcon(":/equipment/img/spine board.jpg"),
                                 "Spine Board");
  ui->comboBoxNameEquip->addItem(QIcon(":/equipment/img/neckcollar.jpg"),
                                 "Neck Collar");
  ui->comboBoxNameEquip->addItem(QIcon(":/equipment/img/suction device.jpg"),
                                 "Suction device");
  ui->comboBoxNameEquip->addItem(
      QIcon(":/equipment/img/stretcher-bed-ramp.jpg"), "Stretcher ramp");
  ui->comboBoxNameEquip->addItem(QIcon(":/equipment/img/firstaid.png"),
                                 "First aid kit");
  ui->comboBoxNameEquip->addItem(
      QIcon(":/equipment/img/blankets and pillows.jpeg"), "Blankets");
  ui->comboBoxNameEquip->addItem(QIcon(":/equipment/img/gloves.jpg"),
                                 "gloves and gowns");
  ui->tableViewEquip->verticalHeader()->hide();

  QStringList items;
  items << "E_ID"
        << "QUANTITY"
        << "STATUS";

  // Add the list of items to the QComboBoxFilter
  ui->comboBoxFilterEquip->addItems(items);
  // Assuming you have a QComboBox instance called comboBox

  ui->spinBoxQuantityEquip->setRange(-1000, 1000);

  // qDebug() << ui->comboBoxFilterEquip->currentText();
  FilterEquip = ui->comboBoxFilterEquip->currentText();

  ui->tableViewEquip->setModel(Etmp.search3(SearchEquip, FilterEquip));
}
void MainWindow::setWindowHospital() {
  ui->tableViewHospital->setModel(hos.read());
}
void MainWindow ::setWindowInterface() {

  setWindowFlag(Qt::FramelessWindowHint);
  ui->SizeGrip->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  ui->MenuBtn->setCheckable(true);
  ui->MenuBtn->setChecked(false);

  // Setup custom control buttons

  connect(ui->CloseBtn, &QPushButton::clicked, this, &QMainWindow::close);
  connect(ui->RestoreBtn, &QPushButton::clicked, this,
          [this]() { isMaximized() ? showNormal() : showMaximized(); });
  connect(ui->MinimizeBtn, &QPushButton::clicked, this,
          &QMainWindow::showMinimized);
  // Setup the button group

  QFrame *tabsButtonsFrame = ui->LeftMenuSubContainer->findChild<QFrame *>(
      "frame_2", Qt::FindDirectChildrenOnly);
  QStackedWidget *wStacked = ui->stackedWidget_3;
  QButtonGroup *buttonGroup = new QButtonGroup(this);

  foreach (QPushButton *wBut, tabsButtonsFrame->findChildren<QPushButton *>(
                                  QString(), Qt::FindDirectChildrenOnly)) {
    wBut->setCheckable(true);
    buttonGroup->addButton(wBut);
  }

  // Check the first button

  tabsButtonsFrame
      ->findChild<QPushButton *>("HomeBtn", Qt::FindDirectChildrenOnly)
      ->setChecked(true);

  // Setup the  sub menu buttons group

  QFrame *moTabsButtonsFrame = ui->LeftMenuSubContainer->findChild<QFrame *>(
      "frame_3", Qt::FindDirectChildrenOnly);
  QStackedWidget *mowstacked = ui->stackedWidget;
  QButtonGroup *moButtonGroup = new QButtonGroup(this);

  foreach (QPushButton *mowbut, moTabsButtonsFrame->findChildren<QPushButton *>(
                                    QString(), Qt::FindDirectChildrenOnly)) {
    mowbut->setCheckable(true);
    moButtonGroup->addButton(mowbut);
  }
  //      qDebug() << moButtonGroup->buttons();

  // Connect button group to stacked widget

  connect(
      buttonGroup, qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked),
      wStacked, [=](QAbstractButton *clicked) {
        // Update Title

        setWindowTitle(clicked->objectName().remove("Btn") +
                       " - Smart Ambulance Agency");

        // Change page

        const QString pageName = clicked->objectName().replace("Btn", "Page");

        QWidget *targetPage = wStacked->findChild<QWidget *>(
            pageName, Qt::FindDirectChildrenOnly);

        wStacked->setCurrentWidget(targetPage);
      });

  // click the home page

  ui->HomeBtn->click();

  // Connect 2nd button's group to another stacked widget

  connect(moButtonGroup,
          qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked),
          mowstacked, [=](QAbstractButton *clicked) {
            QString pageName = clicked->objectName().replace("Btn", "Page");
            QWidget *mowPage = mowstacked->findChild<QWidget *>(
                pageName, Qt::FindDirectChildrenOnly);
            ui->CenterMenuContainer->show();
            mowstacked->setCurrentWidget(mowPage);
          });

  connect(ui->CloseMoBtn, &QPushButton::clicked, ui->CenterMenuContainer,
          [this]() { ui->CenterMenuContainer->hide(); });

  connect(ui->CloseNotifBtn, &QPushButton::clicked,
          ui->PopupNotificationContainer,
          [this]() { ui->PopupNotificationContainer->hide(); });

  connect(ui->MenuBtn, &QPushButton::clicked, ui->LeftMenuContainer,
          [this](bool checked) {
            // Create a property animation object
            QPropertyAnimation *animation =
                new QPropertyAnimation(ui->LeftMenuContainer, "maximumWidth");

            const int endValue = checked ? 50 : 130;
            const int duration = 500;

            // Set the target value of the animation
            animation->setEndValue(endValue);

            // Set the duration of the animation in milliseconds
            animation->setDuration(duration);

            // Start the animation
            animation->start();
          });
}
void MainWindow::Equipment_CRUD() { // CREATE
  connect(ui->AddEquipBtn, &QPushButton::clicked, this, [this]() {
    Equipment E = GetEquipmentFromForm();

    bool test = E.Create();
    if (test) {
      ui->tableViewEquip->setModel(Etmp.search3(SearchEquip, FilterEquip));
      QMessageBox ::information(
          nullptr, QObject::tr("Ok"),
          QObject::tr("Insert done\n Click cancel to exist"),
          QMessageBox::Cancel);
      ui->spinBoxSerialEquip->clear();
      ui->spinBoxQuantityEquip->clear();
      ui->lineEditManufacEquip->clear();
      ui->lineEditIdEquip->clear();
      ui->plainTextEditInfoEquip->clear();
      ui->dateEditpEquip->clear();
      ui->dateEditeEquip->clear();
    } else {
      QMessageBox ::critical(
          nullptr, QObject::tr("Not Ok"),
          QObject::tr("Insert Not done\n Click cancel to exist"),
          QMessageBox::Cancel);
    }
  });

  // Double click for Update
  connect(ui->tableViewEquip, &QTableView::doubleClicked, this,
          [this](const QModelIndex &index) {
            QString id = ui->tableViewEquip->model()
                             ->data(index.sibling(index.row(), 0))
                             .toString();
            QString name = ui->tableViewEquip->model()
                               ->data(index.sibling(index.row(), 1))
                               .toString();
            int serial = ui->tableViewEquip->model()
                             ->data(index.sibling(index.row(), 2))
                             .toInt();
            QString manu = ui->tableViewEquip->model()
                               ->data(index.sibling(index.row(), 3))
                               .toString();
            QDate p = ui->tableViewEquip->model()
                          ->data(index.sibling(index.row(), 4))
                          .toDate();
            QDate e = ui->tableViewEquip->model()
                          ->data(index.sibling(index.row(), 5))
                          .toDate();
            QString s = ui->tableViewEquip->model()
                            ->data(index.sibling(index.row(), 6))
                            .toString();
            QString i = ui->tableViewEquip->model()
                            ->data(index.sibling(index.row(), 7))
                            .toString();
            int q = ui->tableViewEquip->model()
                        ->data(index.sibling(index.row(), 8))
                        .toInt();

            ui->lineEditIdEquip->setText(id);
            ui->spinBoxQuantityEquip->setValue(q);
            ui->spinBoxSerialEquip->setValue(serial);
            ui->lineEditManufacEquip->setText(manu);
            ui->comboBoxNameEquip->setCurrentText(name);
            ui->comboBoxStatusEquip->setCurrentText(s);
            ui->dateEditpEquip->setDate(p);
            ui->dateEditpEquip->setDate(e);
            ui->plainTextEditInfoEquip->setPlainText(i);
          });

  // Delete Equipment
  connect(ui->DeleteEquipBtn, &QPushButton::clicked, [=]() {
    QModelIndex index = ui->tableViewEquip->currentIndex();

    // Retrieve the data for the first column of the selected row
    QVariant data =
        ui->tableViewEquip->model()->data(index.sibling(index.row(), 0));
    int id = data.toInt();

    bool test = Etmp.Delete(id);
    if (test) {
      ui->tableViewEquip->setModel(Etmp.search3(SearchEquip, FilterEquip));
      QMessageBox::information(
          nullptr, QObject::tr("Ok"),
          QObject::tr("Delete done\n Click cancel to exist"),
          QMessageBox::Cancel);
    } else {
      QMessageBox::critical(nullptr, QObject::tr("Ok"),
                            QObject::tr("Delete done\n Click cancel to exist"),
                            QMessageBox::Cancel);
    }
  });

  // UPDATE
  connect(ui->UpdateEquipBtn, &QPushButton::clicked, this, [this]() {
    Equipment E = GetEquipmentFromForm();
    if (E.getquantity() < 20) {
      QString name = "Equipment" + E.getname() + "less than 5";

      QIcon icon(":/equipment/img/ambulance.png");
      QSystemTrayIcon trayicon(icon);
      trayicon.show();
      trayicon.showMessage("Equipments alert", name);
    }
    if (on_lineEditManufacEquip_editingFinished()) {
      bool test = E.Update();
      if (test) {
        ui->tableViewEquip->setModel(Etmp.search3(SearchEquip, FilterEquip));
        QMessageBox::information(
            nullptr, QObject::tr("Ok"),
            QObject::tr("Update done\n Click cancel to exist"),
            QMessageBox::Cancel);
      } else {
        QMessageBox::information(
            nullptr, QObject::tr("not ok\n"),
            QObject::tr("update not done\n Click cancel to exist"),
            QMessageBox::Cancel);
      }
    } else {
      QMessageBox::information(nullptr, QObject::tr("CONTROL\n"),
                               QObject::tr("do not enter special characters"),
                               QMessageBox::Cancel);
    }
  });

  // EXPORT
  connect(ui->ExportEquipBtn, &QPushButton::clicked, this, [this]() {
    // exportToPdf(ui->tableView)  ;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save PDF"), "",
                                                    tr("PDF Files (*.pdf)"));

    if (!fileName.isEmpty()) {
      QPrinter printer;
      printer.setPageOrientation(QPageLayout::Landscape);
      printer.setOutputFileName(fileName);
      // printer.setPageSize(QPageSize::A4);
      printer.setOutputFormat(QPrinter::PdfFormat);

      QTableView tableView;
      tableView.setMinimumSize(1200,
                               900); // Set the minimum size to 800x600 pixels
      tableView.setMaximumSize(1200,
                               900); // Set the maximum size to 1200x900 pixels
      tableView.setModel(
          ui->tableViewEquip
              ->model()); // Copy the model from the original table view

      // Set the width of each column manually
      tableView.setColumnWidth(
          0, 120); // Adjust the column index and width as needed
      tableView.setColumnWidth(1, 120);
      tableView.setColumnWidth(2, 120);
      tableView.setColumnWidth(3, 120);
      tableView.setColumnWidth(4, 120);
      tableView.setColumnWidth(5, 120);
      tableView.setColumnWidth(6, 120);
      tableView.setColumnWidth(7, 120);

      // ...

      QPainter painter(&printer);
      painter.setRenderHints(QPainter::Antialiasing |
                             QPainter::TextAntialiasing |
                             QPainter::SmoothPixmapTransform);
      tableView.render(&painter);
      painter.end();

      QMessageBox::information(
          this, tr("Exported!"),
          tr("Equipment list is now available in PDF format."));
    }
  });

  // STAT
  connect(ui->StatisticsEquipBtn, &QPushButton::clicked, this, [=]() {
    Equipment E;
    QChartView *chart = E.equipment_stat();

    // Show a file dialog to the user to select the export file
    QString exportFilePath = QFileDialog::getSaveFileName(
        this, tr("Export Chart"), QString(), tr("Images (*.png)"));

    if (!exportFilePath.isEmpty()) {
      // Create a pixmap of the chart and save it to the selected file
      QPixmap chartPixmap = chart->grab();
      chartPixmap.save(exportFilePath);
    }

    delete chart;
  });
}

// Input Control of manufacturer
bool MainWindow::on_lineEditManufacEquip_editingFinished() {
  bool local_test_manufacturer = true;
  QString manu = ui->lineEditManufacEquip->text();
  for (int i = 0; i < manu.length(); i++) {
    if (manu[i] == "$" || manu[i] == "&" || manu[i] == "*" || manu[i] == ",")
      local_test_manufacturer = false;
  }
  if (local_test_manufacturer == false) {
    ui->lineEditManufacEquip->setStyleSheet("background-color: red;");
    ui->lineEditManufacEquip->show();
  } else {
    ui->lineEditManufacEquip->setStyleSheet("background-color: white;");
    ui->lineEditManufacEquip->show();
  }
  return local_test_manufacturer;
}
Equipment MainWindow::GetEquipmentFromForm() {
  int id = ui->lineEditIdEquip->text().toInt();
  QString name = ui->comboBoxNameEquip->currentText();
  QString status = ui->comboBoxStatusEquip->currentText();
  int qu = ui->spinBoxQuantityEquip->value();
  int serial = ui->spinBoxSerialEquip->value();
  QString manu = ui->lineEditManufacEquip->text();
  QString info = ui->plainTextEditInfoEquip->toPlainText();
  QString purchase = ui->dateEditpEquip->text();
  QString expire = ui->dateEditeEquip->text();
  Equipment E(id, name, serial, manu, purchase, expire, status, info, qu);

  return E;
}

void MainWindow::on_comboBoxFilterEquip_activated(const QString &arg1) {
  FilterEquip = arg1;

  //    ui->tableViewEquip->setModel(Etmp.read_sortby(arg1)) ;
  ui->tableViewEquip->setModel(Etmp.search3(SearchEquip, FilterEquip));
}

void MainWindow::on_lineEditSearchEquip_textChanged(const QString &arg1) {
  SearchEquip = arg1;

  ui->tableViewEquip->setModel(Etmp.search3(SearchEquip, FilterEquip));
}

void MainWindow::setWindowEmergency() {

  const QMap<QString, Event::Column> cboxToCol{
      {"Id", Event::Column::Id},
      {"Name", Event::Column::PatientName},
      {"Date", Event::Column::CallDate},
  };

  const auto queryColumn = [this, cboxToCol]() -> Event::Column {
    return cboxToCol[ui->emergenciesSortCriteria->currentText()];
  };

  // -------switching interaces---------------

  emergencyInterfaceButtonGroup = new QButtonGroup(ui->EmergencyPage);

  for (auto but : {ui->emergencyListButton, ui->emergencyFormButton}) {
    but->setCheckable(true);
    // but->setChecked(false);
    emergencyInterfaceButtonGroup->addButton(but);
  }

  connect(emergencyInterfaceButtonGroup,
          qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked),
          ui->emergencyStackedWidget, [this](QAbstractButton *clicked) {
            // qDebug() << clicked->isChecked();
            const QString pageName =
                clicked->objectName().replace("Button", "Page");
            QWidget *wPage = ui->emergencyStackedWidget->findChild<QWidget *>(
                pageName, Qt::FindDirectChildrenOnly);
            ui->emergencyStackedWidget->setCurrentWidget(wPage);
          });

  // start with the form

  ui->emergencyFormButton->click();

  // -----------------------------

  // emergencyRegulationRadioGroup = new QButtonGroup;

  // foreach (
  //     QRadioButton *butt,
  //     ui->emergencyRegulationgroupBox->findChildren<QRadioButton *>())
  //  emergencyRegulationRadioGroup->addButton(butt);

  // connect(emergencyRegulationRadioGroup,
  //         qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked),
  //         this, [this](QAbstractButton *clicked) {
  //           qDebug() << "clicked"
  //                    << clicked->text();
  //         });

  // list emergencies

  emergenciesModel = new QSqlQueryModel;

  emergenciesModel->setQuery(Event::Read(QString(), queryColumn()));

  ui->emergenciesTableView->setModel(emergenciesModel);

  // CRUD Buttons

  connect(ui->emergencySaveButton, &QPushButton::released, this, [=]() {
    const Event emergency = eventFromForm();

    // qDebug() << emergency.getEMID();
    // qDebug() << emergency.getcphonenumber();

    if (!emergency.Create()) {
      return;
    }

    // refresh

    emergenciesModel->setQuery(emergenciesModel->query().lastQuery());
  });

  // refresh on search

  connect(ui->emergenciesSearchField, &QLineEdit::textChanged,
          ui->emergenciesTableView, [this, queryColumn](const QString &search) {
            emergenciesModel->setQuery(Event::Read(search, queryColumn()));
          });

  // delete

  connect(ui->emergencyDeleteButton, &QPushButton::released, this, [=]() {
    if (!emergenciesModel->query().isValid()) {
      return;
    }

    const uint row = ui->emergenciesTableView->currentIndex().row();
    const uint id =
        // ui->emergenciesTableView->model()->index(row, 0).data().toUInt();
        ui->emergenciesTableView->model()
            ->index(row, emergenciesModel->record().indexOf("EM_ID"))
            .data()
            .toUInt();

    Event::Delete(id);

    // refresh table

    emergenciesModel->setQuery(emergenciesModel->query().lastQuery());
  });

  // double click

  connect(ui->emergenciesTableView, &QTableView::activated, this,
          [this](const QModelIndex &index) {
            // go to the form page

            ui->emergencyFormButton->click();

            loadTableEventOnForm();
          });

  // updating

  connect(ui->emergencyUpdateButton, &QPushButton::released, this, [this]() {
    const Event emergency = eventFromForm();

    // qDebug() << emergency.getEMID();
    // qDebug() << emergency.getcphonenumber();

    if (!emergency.Update()) {
      return;
    }

    // refresh table

    emergenciesModel->setQuery(emergenciesModel->query().lastQuery());
  });
  // sort
  connect(ui->emergenciesSortCriteria, &QComboBox::currentTextChanged, this,
          [this, queryColumn](const QString &col) {
            emergenciesModel->setQuery(
                Event::Read(ui->emergenciesSearchField->text(), queryColumn()));
          });

  // export
  connect(ui->emergenciesPdfExportButton, &QPushButton::clicked, this,
          [this]() {
            QModelIndex index = ui->emergenciesTableView->currentIndex();

            // Retrieve the data for the first column of the selected row
            QVariant data =
                ui->emergenciesTableView->model()->data(index.sibling(
                    index.row(), emergenciesModel->record().indexOf("EM_ID")));
            int id = data.toInt();

            QString fileName =
                QFileDialog::getSaveFileName(this, tr("Save PDF"), "", "*.pdf");
            Event::exportEmergency(id, fileName);
          });
  // stat
  connect(ui->emergenciesStatisticsButton, &QPushButton::clicked,
          [=]() { Event::statEmergency(); });
}

Event MainWindow::eventFromForm() {

  QStringList medicalConditions;
  foreach (QCheckBox *box,
           ui->emergencyMedicalChecklistGroupBox->findChildren<QCheckBox *>())
    if (box->isChecked())
      medicalConditions.append(box->text());

  const auto checkedRegulationDecision =
                 ui->emergencyRegulationRadioButtonGroup->checkedButton(),
             checkedPatientState =
                 ui->emergencyPatientStateButtonGroup->checkedButton();

  const int id = ui->emergencyIDLineEdit->text().toUInt();
  const int callerPhone = ui->emergencyPhoneLineEdit->text().toUInt();
  const int age = ui->emergencyPatientAgeLineEdit->text().toUInt();
  const int duration = ui->emergencyPatientDurationLineEdit->text().toUInt();
  const QString callType = ui->emergencyTypeLineEdit->text(),
                callerName = ui->emergencyCallerNameLineEdit->text(),
                relationship = ui->emergencyRelationshipLineEdit->text(),
                location = ui->emergencyLocationLineEdit->text(),
                reason = ui->emergencyReasonLineEdit->text(),
                patientName = ui->emergencyPatientNameLineEdit->text(),
                currentState = checkedPatientState != nullptr
                                   ? checkedPatientState->text()
                                   : QString(),
                callDate = ui->emergencyPatientCallDateEdit->text(),
                pathologyRisk = ui->emergencyPathologyRiskLineEdit->text(),
                pathologyType =
                    ui->emergencyPathologyTypeComboBox->currentText(),
                regulationDecision = checkedRegulationDecision != nullptr
                                         ? checkedRegulationDecision->text()
                                         : QString(),
                interventionDate =
                    ui->emergencyPatientInterventionDateEdit->text(),

                medicalCheckList = medicalConditions.join(", ");

  const int hospitalId = ui->emergencyPatientHospitalLineEdit->text().toUInt(),
            ambulanceId =
                ui->emergencyPatientAmbulanceLineEdit->text().toUInt(),
            accountId = user.getId();

  return Event(id, callerPhone, callType, callerName, relationship, location,
               reason, patientName, age, medicalCheckList, currentState,
               callDate, pathologyRisk, pathologyType, interventionDate,
               duration, regulationDecision, hospitalId, ambulanceId,
               accountId);
}

void MainWindow::loadTableEventOnForm() {

  const int row =
      ui->emergenciesTableView->selectionModel()->currentIndex().row();
  const QSqlRecord record = emergenciesModel->record(row);

  // Fill all lineEdits accordingly

  for (const auto &[lineEdit, val] : std::unordered_map<QLineEdit *, QString>{
           {ui->emergencyIDLineEdit, "EM_ID"},
           {ui->emergencyPhoneLineEdit, "C_PHONE_NUMBER"},
           {ui->emergencyPatientAgeLineEdit, "P_AGE"},
           {ui->emergencyTypeLineEdit, "C_TYPE"},
           {ui->emergencyCallerNameLineEdit, "C_NAME"},
           {ui->emergencyPatientNameLineEdit, "P_NAME"},
           {ui->emergencyRelationshipLineEdit, "RELATIONSHIP"},
           {ui->emergencyReasonLineEdit, "REASON"},

           {ui->emergencyPathologyRiskLineEdit, "PATHOLOGY_RISKS"},

           {ui->emergencyLocationLineEdit, "LOCATION"},
           {ui->emergencyPatientAmbulanceLineEdit, "A_ID"},
           {ui->emergencyPatientHospitalLineEdit, "H_ID"},
           {ui->emergencyPatientDurationLineEdit, "DURATION"},

       }) {
    lineEdit->setText(record.value(val).toString());
  }

  // date
  ui->emergencyPatientCallDateEdit->setDate(record.value("CALL_DT").toDate());
  ui->emergencyPatientInterventionDateEdit->setDate(
      record.value("INTERVENTION_DT").toDate());

  // Fill medical checklist

  // uncheck everything

  foreach (auto box,
           ui->emergencyMedicalChecklistGroupBox->findChildren<QCheckBox *>()) {
    box->setChecked(false);
  }

  const QStringList medicalConditions =
      record.value("MEDICAL_CHECKLIST").toString().split(", ");

  for (const QString &condition : medicalConditions)
    if (const auto box =
            ui->emergencyMedicalChecklistGroupBox->findChild<QCheckBox *>(
                "emergencyPatientMedicalCondition" + condition);
        box != nullptr)
      box->setChecked(true);

  // Regulation

  if (const QString checkedRegulationDecision =
          record.value("REGULATION_DECISION").toString();
      !checkedRegulationDecision.isEmpty()) {
    if (const auto regulationRadioButton =
            ui->emergencyRegulationgroupBox->findChild<QRadioButton *>(
                "emergencyRegulationRadio" +
                checkedRegulationDecision.split(' ').join(
                    "")); // only way without creating a new var
        regulationRadioButton != nullptr) {
      regulationRadioButton->setChecked(true);
    }
  } else {
    // Default value
    ui->emergencyRegulationRadioEmergency->setChecked(true);
  }

  // Patient state

  if (const auto patientState = record.value("CURRENT_STATE").toString();
      !patientState.isEmpty()) {
    if (const auto patientStateRadioButton =
            ui->emergencyPatientStateWidget->findChild<QRadioButton *>(
                "emergencyPatient" + patientState + "RadioButton");
        patientStateRadioButton != nullptr) {
      patientStateRadioButton->setChecked(true);
    }
  } else {
    // Default value
    ui->emergencyPatientConsciousRadioButton->setChecked(true);
  }

  // Pathology type

  ui->emergencyPathologyTypeComboBox->setCurrentText(
      record.value("CURRENT_STATE").toString());
}

void MainWindow::Ambulance_CRUD() {
  // Create Ambulance
  connect(ui->pushButtonAddAmbBtn, &QPushButton::clicked, this, [this]() {
    int year = ui->lineEditDateAmb->text().toInt();
    int reg_number = ui->lineEditRegAmb->text().toInt();
    int A_id = ui->lineEditIdAmb->text().toInt();
    QString type = ui->lineEditTypeAmb->text();
    QString manufacturer = ui->lineEditManAmb->text();
    QString fueltype = ui->lineEditTFAmb->text();

    ambulance E(A_id, reg_number, type, manufacturer, year, fueltype);
    if ((ui->lineEditTypeAmb->text() == "BLS" ||
         ui->lineEditTypeAmb->text() == "MVA")) {
      if ((ui->lineEditTFAmb->text() == "GASOLINE" ||
           ui->lineEditTFAmb->text() == "DIESEL")) {
        bool test = E.Create();

        if (test) {
          // Refresh (Actualiser)
          ui->tableView->setModel(Etmp1.Read());

          QMessageBox::information(nullptr, QObject::tr("OK"),
                                   QObject::tr("insertion done\n"
                                               "Click Cancel to exit."),
                                   QMessageBox::Cancel);

        } else {
          QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                                QObject::tr("insertion failed.\n"
                                            "Click Cancel to exit."),
                                QMessageBox::Cancel);
        }
      } else {
        QMessageBox::information(
            nullptr, QObject::tr("OK"),
            QObject::tr("Fuel type must be either GASOLINE or DIESEL.\n"
                        "Click Cancel to exit."),
            QMessageBox::Cancel);
      }
    } else {
      QMessageBox::information(
          nullptr, QObject::tr("OK"),
          QObject::tr("Type of ambulance must be either BLS or MVA.\n"
                      "Click Cancel to exit."),
          QMessageBox::Cancel);
    }
  });
  // Update Ambulance

  connect(ui->pushButtonUpdateAmbBtn, &QPushButton::clicked, this, [this]() {
    int year = ui->lineEditDateAmb->text().toInt();
    int reg_number = ui->lineEditRegAmb->text().toInt();
    int A_id = ui->lineEditIdAmb->text().toInt();
    QString type = ui->lineEditTypeAmb->text();
    QString manufacturer = ui->lineEditManAmb->text();
    QString fueltype = ui->lineEditTFAmb->text();
    ambulance E(A_id, reg_number, type, manufacturer, year, fueltype);

    bool test = E.Update();

    if (test) {
      // Refresh (Actualiser)
      ui->tableView->setModel(Etmp1.Read());

      QMessageBox::information(nullptr, QObject::tr("OK"),
                               QObject::tr("Update done\n"
                                           "Click Cancel to exit."),
                               QMessageBox::Cancel);

    } else
      QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                            QObject::tr("Update failed.\n"
                                        "Click Cancel to exit."),
                            QMessageBox::Cancel);
  });
  // Delete Ambulance
  connect(ui->pushButtonDeleteeAmbBtn, &QPushButton::clicked, this, [this]() {
    int year = ui->lineEditDateAmb->text().toInt();
    int reg_number = ui->lineEditRegAmb->text().toInt();
    int A_id = ui->lineEditIdAmb->text().toInt();
    QString type = ui->lineEditTypeAmb->text();
    QString manufacturer = ui->lineEditManAmb->text();
    QString fueltype = ui->lineEditTFAmb->text();
    QString id = ui->lineEditIdAmb->text();
    ambulance E(A_id, reg_number, type, manufacturer, year, fueltype);
    bool test = E.Delete(id);

    if (test) {
      // Refresh (Actualiser)
      ui->tableView->setModel(Etmp1.Read());
    } else
      QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                            QObject::tr("delete failed.\n"
                                        "Click Cancel to exit."),
                            QMessageBox::Cancel);
  });
}

void MainWindow::setWindowAmbulance() {

  connect(ui->ambulanceInterfacePicker, &QComboBox::currentTextChanged, this,
          [this](const QString choice) {
            const QString pageName = choice.toLower() + "Page";
            QWidget *wPage = ui->ambulanceInterfaces->findChild<QWidget *>(
                pageName, Qt::FindDirectChildrenOnly);
            ui->ambulanceInterfaces->setCurrentWidget(wPage);
            // qDebug() << pageName;
          });
}
// search using lineedit
void MainWindow::on_pushButtonSearchAmbbtn_clicked() {
  QString idString = ui->lineEditIDSAmbBtn->text();
  int id = idString.toInt();

  // Call the search method with the ID, table view, and ID input line edit
  search(id, ui->tableView, ui->lineEditIDSAmbBtn);
}
void MainWindow::search(int id, QTableView *tableView,
                        QLineEdit *lineEditIDSAmbBtn) {

  QSqlQueryModel *model = Etmp1.Search(id);
  // Display the result in the table view
  tableView->setModel(model);
  // Clear the ID input line edit
  lineEditIDSAmbBtn->clear();
}
// exporting pdf file
void MainWindow::on_pushButtonExportAmbBtn_clicked() {
  ambulance amb;
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("C:/Users/kharr/OneDrive/Bureau"), QDir::currentPath());
  amb.generatePDF();
}
// using the combobox to sort the list
void MainWindow::on_comboBox_currentTextChanged(const QString &arg1) {
  QSqlQueryModel *model = Etmp1.Sort(arg1);

  ui->tableView->setModel(model);
}
// Statisitc button
void MainWindow::on_pushButtonStatAmbBtn_clicked() {
  // Create a chart and set its title
  QChart *chart = new QChart();
  chart->setTitle("Ambulance Information");

  // Create a series to hold the data
  QBarSeries *series = new QBarSeries();
  series->setName("Number of Ambulances");

  // Query the database for the data
  QSqlQuery query;
  query.prepare("SELECT TYPE, COUNT(*) FROM AMBULANCE GROUP BY TYPE");
  if (!query.exec()) {
    qDebug() << "Query failed:" << query.lastError().text();
    return;
  }

  // Loop through the query results and add them to the series
  while (query.next()) {
    QString type = query.value(0).toString();
    int count = query.value(1).toInt();
    qDebug() << "Type:" << type << "Count:" << count;
    QBarSet *set = new QBarSet(type);
    *set << count;
    series->append(set);
  }

  // Add the series to the chart
  chart->addSeries(series);

  // Create the X axis and set its title
  QBarCategoryAxis *axisX = new QBarCategoryAxis();
  axisX->setTitleText("Ambulance Type");
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  // Create the Y axis and set its title
  QValueAxis *axisY = new QValueAxis();
  axisY->setTitleText("Number of Ambulances");
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);

  // Create the chart view and set its chart
  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  // Show the chart view
  chartView->show();
}

void MainWindow::on_tableView_activated(const QModelIndex &index) {
  QString val = ui->tableView->model()->data(index).toString();

  QSqlQuery qry;
  qry.prepare("Select * from Ambulance where A_id='" + val +
              "'or reg_number='" + val + "'");
  if (qry.exec()) {
    while (qry.next()) {
      ui->lineEditIdAmb->setText(qry.value(0).toString());
      ui->lineEditRegAmb->setText(qry.value(1).toString());
      ui->lineEditTypeAmb->setText(qry.value(2).toString());
      ui->lineEditManAmb->setText(qry.value(3).toString());
      ui->lineEditDateAmb->setText(qry.value(4).toString());
      ui->lineEditTFAmb->setText(qry.value(5).toString());
    }
  } else {
    QMessageBox::critical(nullptr, QObject::tr("ok"),
                          QObject::tr("listing failed \n"
                                      "click Cancel to exit."),
                          QMessageBox::Cancel);
  }
}

void MainWindow::Calendar_CRUD() {
  // create calendar
  connect(ui->AddRdvBtn, &QPushButton::clicked, this, [this] {
    int id = 0;
    int cin = ui->lineEditIdvAmb->text().toInt();
    QDate date = ui->date->selectedDate();
    QTime hour = ui->hourRdvAmb->time();
    QTime mint(6, 0, 0, 0);
    QTime maxt(13, 0, 0, 0);
    calendar r(id, date, hour, cin);
    if (((ui->hourRdvAmb->time() >= mint) &&
         (ui->hourRdvAmb->time() <= maxt))) {
      bool test = r.add();
      QSqlQueryModel *model = cl.display();
      ui->RdvlistView->setModel(model);
      highlightStartDates();
      if (test) {
        QMessageBox::information(nullptr, QObject::tr("OK"),
                                 QObject::tr("Add successful.\n"
                                             "Click Cancel to exit."),
                                 QMessageBox::Cancel);
      } else
        QMessageBox::critical(nullptr, QObject::tr("Not ok"),
                              QObject::tr("add unsuccessful.\n"
                                          "Click Cancel to exit."),
                              QMessageBox::Cancel);
    } else {
      QMessageBox::information(
          nullptr, QObject::tr("OK"),
          QObject::tr("Time must be between 6:00AM-13:00PM.\n"
                      "Click Cancel to exit."),
          QMessageBox::Cancel);
    }
  });

  // Delete Calendar
  connect(ui->DeleteRdvBtn, &QPushButton::clicked, this, [this] {
    calendar r;

    QVariant rowData =
        ui->RdvlistView->model()->data(ui->RdvlistView->currentIndex());

    qDebug() << "selected line is : " << rowData.toString();
    if (!(rowData.isNull())) {
      bool test = r.Delete(rowData.toInt());
      if (test) {
        highlightStartDates();
        QMessageBox::information(nullptr, QObject::tr("ok"),
                                 QObject::tr("delete done.\n"
                                             "Click Cancel to exit."),
                                 QMessageBox::Cancel);
        QSqlQueryModel *model = cl.display();
        ui->RdvlistView->setModel(model);

      } else
        QMessageBox::critical(nullptr, QObject::tr("not ok"),
                              QObject::tr("delete not done.\n"
                                          "Click Cancel to exit."),
                              QMessageBox::Cancel);

    } else {
      QMessageBox::information(nullptr, QObject::tr("ok"),
                               QObject::tr("Select id to Delete please!!.\n"
                                           "Click Cancel to exit."),
                               QMessageBox::Cancel);
    }
  });
}
void MainWindow::highlightStartDates() {
  ui->date->setDateTextFormat(QDate(), QTextCharFormat());
  QMap<QDate, QList<QString>> projectDates;
  // Clear the projectDates map
  projectDates.clear();

  // Retrieve the start dates of all projects from the tableView
  for (int row = 0; row < ui->RdvlistView->model()->rowCount(); row++) {
    QDate startDate =
        ui->RdvlistView->model()->index(row, 2).data(Qt::DisplayRole).toDate();
    QString projectName = ui->RdvlistView->model()
                              ->index(row, 1)
                              .data(Qt::DisplayRole)
                              .toString();
    projectDates[startDate].append(projectName + " (start)");
  }

  // Set the format for start dates in the calendar widget
  QTextCharFormat startFormat;
  startFormat.setBackground(Qt::green);
  startFormat.setForeground(Qt::white);

  foreach (auto date, projectDates.keys()) {
    QList<QString> projectNames = projectDates.value(date);
    QString text = projectNames.join(", ");
    if (text.endsWith("(start)")) {
      ui->date->setDateTextFormat(
          date, startFormat); // Highlight start date with green color
    }
  }
}

void MainWindow::Hospital_CRUD() {

  // CREATE
  connect(ui->AddHospitalBtn, &QPushButton::clicked, this, [this]() {
    int id = ui->lineEditIdHospital->text().toInt();
    int nb_beds = ui->spinBoxBedsHospital->value();
    int nb_patients = ui->spinBoxPatientsHospital->value();
    int phone = ui->lineEditPhoneHospital->text().toInt();
    QString name = ui->lineEditNameHospital->text();
    QString email = ui->lineEditEmailHospital->text();
    QString adress = ui->lineEditAddressHospital->text();
    QString speciality = ui->comboBoxSpecialityHospital->currentText();
    QString type;
    if (ui->radioButtonPrivateHospital->isChecked()) {
      type = "Private";
    }
    if (ui->radioButtonPublicHospital->isChecked()) {
      type = "Public";
    }
    QRegExp rx("\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Z|a-z]{2,}\\b");
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    rx.setPatternSyntax(QRegExp::RegExp);
    if (!rx.exactMatch(email)) {
      QMessageBox::warning(this, tr("Warning"), tr("Invalid email format."),
                           QMessageBox::Ok);
      return;
    }
    QStringList selectedOptions;
    if (ui->checkBoxICUHospital->isChecked()) {
      selectedOptions.append("ICU");
    }
    if (ui->checkBoxDiagnosticHospital->isChecked()) {
      selectedOptions.append("Diagnostic");
    }
    if (ui->checkBoxRehabHospital->isChecked()) {
      selectedOptions.append("Rehab");
    }
    if (ui->checkBoxSurgicalHospital->isChecked()) {
      selectedOptions.append("Surgical");
    }
    if (ui->checkBoxEmergencyHospital->isChecked()) {
      selectedOptions.append("Emergency");
    }
    if (ui->checkBoxUnitsHospital->isChecked()) {
      selectedOptions.append("Inpatient");
    }
    if (ui->checkBoxOutpatientHospital->isChecked()) {
      selectedOptions.append("Outpatient");
    }
    if (ui->checkBoxMentalHospital->isChecked()) {
      selectedOptions.append("mental health");
    }
    QString s_id = selectedOptions.join(", ");
    hospital H(name, adress, type, speciality, email, s_id, phone, nb_beds,
               nb_patients, id);
    bool test = H.create();
    if (test) {
      ui->tableViewHospital->setModel(hos.read());
      QMessageBox::information(nullptr, QObject::tr("OK"),
                               QObject::tr("insert done \n"
                                           "Click Cancel to exit."),
                               QMessageBox::Cancel);
    } else {
      QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                            QObject::tr("insert failed.\n"
                                        "Click Cancel to exit."),
                            QMessageBox::Cancel);
    }
  });
  // Double click display
  connect(ui->tableViewHospital, &QTableView::doubleClicked, this,
          [this](const QModelIndex &index) {
            QString type = ui->tableViewHospital->model()
                               ->data(index.sibling(index.row(), 6))
                               .toString();

            QString str = ui->tableViewHospital->model()
                              ->data(index.sibling(index.row(), 9))
                              .toString();

            ui->checkBoxICUHospital->setChecked(false);
            ui->checkBoxDiagnosticHospital->setChecked(false);
            ui->checkBoxEmergencyHospital->setChecked(false);
            ui->checkBoxMentalHospital->setChecked(false);
            ui->checkBoxOutpatientHospital->setChecked(false);
            ui->checkBoxRehabHospital->setChecked(false);
            ui->checkBoxUnitsHospital->setChecked(false);
            ui->checkBoxSurgicalHospital->setChecked(false);

            ui->lineEditIdHospital->setText(
                ui->tableViewHospital->model()
                    ->data(index.sibling(index.row(), 0))
                    .toString());
            ui->lineEditAddressHospital->setText(
                ui->tableViewHospital->model()
                    ->data(index.sibling(index.row(), 2))
                    .toString());
            ui->lineEditEmailHospital->setText(
                ui->tableViewHospital->model()
                    ->data(index.sibling(index.row(), 4))
                    .toString());
            ui->lineEditNameHospital->setText(
                ui->tableViewHospital->model()
                    ->data(index.sibling(index.row(), 1))
                    .toString());
            ui->lineEditPhoneHospital->setText(
                ui->tableViewHospital->model()
                    ->data(index.sibling(index.row(), 3))
                    .toString());
            ui->spinBoxBedsHospital->setValue(
                ui->tableViewHospital->model()
                    ->data(index.sibling(index.row(), 7))
                    .toInt());
            ui->spinBoxPatientsHospital->setValue(
                ui->tableViewHospital->model()
                    ->data(index.sibling(index.row(), 8))
                    .toInt());
            ui->comboBoxSpecialityHospital->setCurrentText(
                ui->tableViewHospital->model()
                    ->data(index.sibling(index.row(), 5))
                    .toString());
            if (type == "Private")
              ui->radioButtonPrivateHospital->setChecked(true);
            else
              ui->radioButtonPublicHospital->setChecked(true);

            QStringList wordsToFind = {
                "ICU",        "Emergency", "Rehab",         "Diagnostic",
                "Outpatient", "Inpatient", "Mental health", "Surgical"};

            foreach (const QString &word, wordsToFind) {
              int index = str.indexOf(word, 0, Qt::CaseInsensitive);
              if (index != -1) {
                if (word == "ICU") {
                  ui->checkBoxICUHospital->setChecked(true);
                } else if (word == "Emergency") {
                  ui->checkBoxEmergencyHospital->setChecked(true);
                } else if (word == "Rehab") {
                  ui->checkBoxRehabHospital->setChecked(true);
                } else if (word == "Diagnostic") {
                  ui->checkBoxDiagnosticHospital->setChecked(true);
                } else if (word == "Outpatient") {
                  ui->checkBoxOutpatientHospital->setChecked(true);
                } else if (word == "Inpatient") {
                  ui->checkBoxUnitsHospital->setChecked(true);
                } else if (word == "Mental health") {
                  ui->checkBoxMentalHospital->setChecked(true);
                } else if (word == "Surgical") {
                  ui->checkBoxSurgicalHospital->setChecked(true);
                }
              }
            }
          });

  // DELETE
  connect(ui->DeleteHospitalBtn, &QPushButton::clicked, this, [=]() {
    QModelIndex index = ui->tableViewHospital->currentIndex();

    // Retrieve the data for the first column of the selected row
    QVariant data =
        ui->tableViewHospital->model()->data(index.sibling(index.row(), 0));
    int id = data.toInt();

    bool test = hos.Delete(id);
    if (test) {
      ui->tableViewHospital->setModel(hos.read());
      QMessageBox::information(
          nullptr, QObject::tr("Ok"),
          QObject::tr("Delete done\n Click cancel to exist"),
          QMessageBox::Cancel);
    } else {
      QMessageBox::critical(nullptr, QObject::tr("Ok"),
                            QObject::tr("Delete done\n Click cancel to exist"),
                            QMessageBox::Cancel);
    }
  });

  // UPDATE

  connect(ui->UpdateHospitalBtn, &QPushButton::clicked, this, [=]() {
    int id = ui->lineEditIdHospital->text().toInt();
    int nb_beds = ui->spinBoxBedsHospital->value();
    int nb_patients = ui->spinBoxPatientsHospital->value();
    int phone = ui->lineEditPhoneHospital->text().toInt();
    QString name = ui->lineEditNameHospital->text();
    QString email = ui->lineEditEmailHospital->text();
    QString adress = ui->lineEditAddressHospital->text();
    QString speciality = ui->comboBoxSpecialityHospital->currentText();
    QString type;
    if (ui->radioButtonPrivateHospital->isChecked()) {
      type = "Private";
    }
    if (ui->radioButtonPublicHospital->isChecked()) {
      type = "Public";
    }
    QRegExp rx("\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Z|a-z]{2,}\\b");
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    rx.setPatternSyntax(QRegExp::RegExp);
    if (!rx.exactMatch(email)) {
      QMessageBox::warning(this, tr("Warning"), tr("Invalid email format."),
                           QMessageBox::Ok);
      return;
    }
    QStringList selectedOptions;
    if (ui->checkBoxICUHospital->isChecked()) {
      selectedOptions.append("ICU");
    }
    if (ui->checkBoxDiagnosticHospital->isChecked()) {
      selectedOptions.append("Diagnostic");
    }
    if (ui->checkBoxRehabHospital->isChecked()) {
      selectedOptions.append("Rehab");
    }
    if (ui->checkBoxSurgicalHospital->isChecked()) {
      selectedOptions.append("Surgical");
    }
    if (ui->checkBoxEmergencyHospital->isChecked()) {
      selectedOptions.append("Emergency");
    }
    if (ui->checkBoxUnitsHospital->isChecked()) {
      selectedOptions.append("Inpatient");
    }
    if (ui->checkBoxOutpatientHospital->isChecked()) {
      selectedOptions.append("Outpatient");
    }
    if (ui->checkBoxMentalHospital->isChecked()) {
      selectedOptions.append("mental health");
    }
    QString s_id = selectedOptions.join(", ");
    hospital H(name, adress, type, speciality, email, s_id, phone, nb_beds,
               nb_patients, id);
    bool test = H.Update();

    if (test) {
      // Refresh (Actualiser)
      ui->tableViewHospital->setModel(hos.read());

      QMessageBox::information(nullptr, QObject::tr("OK"),
                               QObject::tr("Update done\n"
                                           "Click Cancel to exit."),
                               QMessageBox::Cancel);

    } else {
      QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                            QObject::tr("Update failed.\n"
                                        "Click Cancel to exit."),
                            QMessageBox::Cancel);
    }
  });

  // search
  connect(ui->lineEditSearchHospital, &QLineEdit::textChanged, this,
          [=](const QString &arg1) {
            ui->tableViewHospital->setModel(hos.search(arg1));
          });
  // sort

  connect(ui->comboBox_3, &QComboBox::currentTextChanged, this,
          [this](const QString &arg1) {
            ui->tableViewHospital->setModel(hos.sort(arg1));
          });
  // Export

  connect(ui->ExportHospitalBtn, &QPushButton::clicked, this, [this]() {
    QModelIndex index = ui->tableViewHospital->currentIndex();

    // Retrieve the data for the first column of the selected row
    QVariant data =
        ui->tableViewBill->model()->data(index.sibling(index.row(), 0));
    int id = data.toInt();
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save PDF"), "", "*.pdf");
    hospital::exportHospital(id, fileName);
  });
}

void MainWindow::Bill_CRUD() {
  connect(ui->AddBillBtn, &QPushButton::clicked, this, [this]() {
    // Get information entered in the 3 field
    int b_id = ui->lineEditIdBill->text().toInt();

    int h_id = ui->lineEditHosiptalIdBill->text().toInt();
    int p_id = ui->lineEditEmerIdBill->text().toInt();
    QString state_str = ui->comboBoxStateBill->currentText();
    int state = -1;
    if (state_str == "Payed")
      state = 1;
    else
      state = 0;

    // int value = ui->billValueLineEdit->text().toInt();
    QString fname = ui->lineEditFnameBill->text();
    QString lname = ui->lineEditLnameBill->text();
    QString phone = ui->lineEditNumberBill->text();
    QString type = ui->lineEditEmerTypeBill->text();
    QString insurance = ui->lineEditInsuranceBill->text();

    // Get information entered in the 3 field

    // int value;
    int billValue = 0;
    QString cur_state, patho_type, regulation_desc, type_ambulance;
    int duration = 0;

    QSqlQuery query = bill::GetForeignDetails(p_id);

    if (query.exec() && query.next()) {
      cur_state = query.value(0).toString();
      duration = query.value(1).toInt();
      patho_type = query.value(2).toString();
      regulation_desc = query.value(3).toString();
      type_ambulance = query.value(4).toString();

      qDebug() << "Emergency reason: " << cur_state;
      qDebug() << "duration: " << duration;
      qDebug() << "patho_type: " << patho_type;
      qDebug() << "ambulance: " << type_ambulance;

      // qDebug() << "l id li njb mnha: " << p_id;
    }

    // Calculate the cost of the selected services
    if (ui->checkBoxBasicBill->isChecked())
      billValue += 20;
    if (ui->checkBoxAdvancedBill->isChecked())
      billValue += 40;
    if (ui->checkBoxCriticalBill->isChecked())
      billValue += 60;
    // Calculate the cost based on the time of day
    if (ui->checkBoxPeakBill->isChecked())
      billValue += 20;
    else if (ui->checkBoxOffPeakBill->isChecked())
      billValue += 10;
    // Calculate the cost based on the distance
    if (ui->checkBox10Bill->isChecked())
      billValue += 80;
    if (ui->checkBox20Bill->isChecked())
      billValue += 190;
    if (ui->checkBox30Bill->isChecked())
      billValue += 260;

    if (cur_state == "Conscious") {
      billValue += 5;
    }
    if (cur_state == "Unconscious") {
      billValue += 10;
    }

    if (cur_state == "out") {
      billValue += 10;
    }

    if (patho_type == "2") {
      billValue += 20;
    }

    if (patho_type == "Surgical") {
      billValue += 20;
    }
    if (patho_type == "Medical") {
      billValue += 10;
    }

    if (regulation_desc == "Prc") {
      billValue += 20;
    }
    if (regulation_desc == "Emergency") {
      billValue += 35;
    }
    if (regulation_desc == " Unjustified Demand") {
      billValue += 20;
    }
    if (regulation_desc == " Private Transportation") {
      billValue += 30;
    }

    if (duration <= 15) {
      billValue += 20;
    } else {
      billValue += 10;
    }

    if (type_ambulance == "EMS") {
      billValue += 10;
    }
    if (type_ambulance == "EMT") {
      billValue += 10;
    }
    if (type_ambulance == "z") {
      billValue += 10;
    }

    // Update the bill value in the UI
    ui->lineEditTotalBill->setText(QString::number(billValue) + " TND");

    QString value = QString::number(billValue) + " TND";

    bill H(fname, lname, insurance, phone, type, h_id, b_id, p_id, state,
           value);
    // using information entered in the interface

    bool test = H.create(); // insert the object into the databse Student table
                            // and et the returned value of query.exec()

    if (test) // if the query is executed  ==> QMessageBox::information
    {
      // Refresh
      ui->tableViewBill->setModel(Bi.read());

      QMessageBox::information(nullptr, QObject::tr("OK"),
                               QObject::tr("insert done \n"
                                           "Click Cancel to exit."),
                               QMessageBox::Cancel);
    } else // if the query is not executed  ==> QMessageBox::critical
      QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                            QObject::tr("insert failed.\n"
                                        "Click Cancel to exit."),
                            QMessageBox::Cancel);
  });
  connect(ui->DeleteBillBtn, &QPushButton::clicked, this, [=]() {
    QModelIndex index = ui->tableViewBill->currentIndex();

    // Retrieve the data for the first column of the selected row
    QVariant data =
        ui->tableViewBill->model()->data(index.sibling(index.row(), 0));
    int id = data.toInt();
    bool test = Bi.Delete(id);

    if (test) {
      // Refresh (Actualiser)
      ui->tableViewBill->setModel(Bi.read());

      QMessageBox::information(nullptr, QObject::tr("OK"),
                               QObject::tr("delete done\n"
                                           "Click Cancel to exit."),
                               QMessageBox::Cancel);

    } else
      QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                            QObject::tr("delete failed.\n"
                                        "Click Cancel to exit."),
                            QMessageBox::Cancel);
  });
  // Double Click
  connect(
      ui->tableViewBill, &QTableView::doubleClicked, this,
      [this](const QModelIndex &index) {
        int type = ui->tableViewBill->model()
                       ->data(index.sibling(index.row(), 7))
                       .toInt();

        ui->lineEditIdBill->setText(ui->tableViewBill->model()
                                        ->data(index.sibling(index.row(), 0))
                                        .toString());
        ui->lineEditFnameBill->setText(ui->tableViewBill->model()
                                           ->data(index.sibling(index.row(), 1))
                                           .toString());
        ui->lineEditLnameBill->setText(ui->tableViewBill->model()
                                           ->data(index.sibling(index.row(), 2))
                                           .toString());
        ui->lineEditNumberBill->setText(
            ui->tableViewBill->model()
                ->data(index.sibling(index.row(), 3))
                .toString());
        ui->lineEditEmerTypeBill->setText(
            ui->tableViewBill->model()
                ->data(index.sibling(index.row(), 4))
                .toString());
        ui->lineEditEmerIdBill->setText(
            ui->tableViewBill->model()
                ->data(index.sibling(index.row(), 5))
                .toString());
        ui->lineEditInsuranceBill->setText(
            ui->tableViewBill->model()
                ->data(index.sibling(index.row(), 6))
                .toString());
        if (type == 1)
          ui->comboBoxStateBill->setCurrentIndex(0);
        else
          ui->comboBoxStateBill->setCurrentIndex(1);
        ui->lineEditTotalBill->setText(ui->tableViewBill->model()
                                           ->data(index.sibling(index.row(), 8))
                                           .toString());
        ui->lineEditHosiptalIdBill->setText(
            ui->tableViewBill->model()
                ->data(index.sibling(index.row(), 9))
                .toString());
      });

  // Update

  connect(ui->UpdateBillBtn, &QPushButton::clicked, this, [=]() {
    int b_id = ui->lineEditIdBill->text().toInt();

    int h_id = ui->lineEditHosiptalIdBill->text().toInt();
    int p_id = ui->lineEditEmerIdBill->text().toInt();
    QString state_str = ui->comboBoxStateBill->currentText();
    int state = -1;
    if (state_str == "Payed")
      state = 1;
    else
      state = 0;

    // int value = ui->billValueLineEdit->text().toInt();
    QString fname = ui->lineEditFnameBill->text();
    QString lname = ui->lineEditLnameBill->text();
    QString phone = ui->lineEditNumberBill->text();
    QString type = ui->lineEditEmerTypeBill->text();
    QString insurance = ui->lineEditInsuranceBill->text();

    // Get information entered in the 3 field

    // int value;
    int billValue = 0;
    QString cur_state, patho_type, regulation_desc, type_ambulance;
    int duration = 0;

    QSqlQuery query = bill::GetForeignDetails(p_id);

    if (query.exec() && query.next()) {
      cur_state = query.value(0).toString();
      duration = query.value(1).toInt();
      patho_type = query.value(2).toString();
      regulation_desc = query.value(3).toString();
      type_ambulance = query.value(4).toString();

      qDebug() << "Emergency reason: " << cur_state;
      qDebug() << "duration: " << duration;
      qDebug() << "patho_type: " << patho_type;
      qDebug() << "ambulance: " << type_ambulance;

      // qDebug() << "l id li njb mnha: " << p_id;
    }

    // Calculate the cost of the selected services
    if (ui->checkBoxBasicBill->isChecked())
      billValue += 20;
    if (ui->checkBoxAdvancedBill->isChecked())
      billValue += 40;
    if (ui->checkBoxCriticalBill->isChecked())
      billValue += 60;
    // Calculate the cost based on the time of day
    if (ui->checkBoxPeakBill->isChecked())
      billValue += 20;
    else if (ui->checkBoxOffPeakBill->isChecked())
      billValue += 10;
    // Calculate the cost based on the distance
    if (ui->checkBox10Bill->isChecked())
      billValue += 80;
    if (ui->checkBox20Bill->isChecked())
      billValue += 190;
    if (ui->checkBox30Bill->isChecked())
      billValue += 260;

    if (cur_state == "Conscious") {
      billValue += 5;
    }
    if (cur_state == "Unconscious") {
      billValue += 10;
    }

    if (cur_state == "out") {
      billValue += 10;
    }

    if (patho_type == "2") {
      billValue += 20;
    }

    if (patho_type == "Surgical") {
      billValue += 20;
    }
    if (patho_type == "Medical") {
      billValue += 10;
    }

    if (regulation_desc == "Prc") {
      billValue += 20;
    }
    if (regulation_desc == "Emergency") {
      billValue += 35;
    }
    if (regulation_desc == " Unjustified Demand") {
      billValue += 20;
    }
    if (regulation_desc == " Private Transportation") {
      billValue += 30;
    }

    if (duration <= 15) {
      billValue += 20;
    } else {
      billValue += 10;
    }

    if (type_ambulance == "EMS") {
      billValue += 10;
    }
    if (type_ambulance == "EMT") {
      billValue += 10;
    }
    if (type_ambulance == "z") {
      billValue += 10;
    }

    // Update the bill value in the UI
    ui->lineEditTotalBill->setText(QString::number(billValue) + " TND");

    QString value = QString::number(billValue) + " TND";

    bill H(fname, lname, insurance, phone, type, h_id, b_id, p_id, state,
           value);
    // using information entered in the interface

    bool test = H.Update();
    if (test) {
      // Refresh (Actualiser)
      ui->tableViewBill->setModel(Bi.read());

      QMessageBox::information(nullptr, QObject::tr("OK"),
                               QObject::tr("Update done\n"
                                           "Click Cancel to exit."),
                               QMessageBox::Cancel);

    } else
      QMessageBox::critical(nullptr, QObject::tr("Not OK"),
                            QObject::tr("Update failed.\n"
                                        "Click Cancel to exit."),
                            QMessageBox::Cancel);
  });

  // Search

  connect(ui->lineEditSearchBill, &QLineEdit::textChanged, this,
          [=](const QString &arg1) {
            ui->tableViewBill->setModel(Bi.search(arg1));
          });

  // SORT
  connect(ui->comboBoxFilterBill, &QComboBox::currentTextChanged, this,
          [this](const QString &arg1) {
            ui->tableViewBill->setModel(Bi.sort(arg1));
          });

  // EXPORT

  connect(ui->ExportBillBtn, &QPushButton::clicked, this, [this]() {
    QModelIndex index = ui->tableViewBill->currentIndex();

    // Retrieve the data for the first column of the selected row
    QVariant data =
        ui->tableViewBill->model()->data(index.sibling(index.row(), 0));
    int id = data.toInt();

    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save PDF"), "", "*.pdf");
    bill::exportBill(id, fileName);
  });
  // email
  connect(ui->pushButtonEmailBtn, &QPushButton::clicked, [=]() {
    Dialog *mailDialog = new Dialog(this);
    mailDialog->setModal(true);
    mailDialog->exec();
    delete mailDialog;
  });
  // STAT
  connect(ui->StatisticsBillBtn, &QPushButton::clicked,
          [=]() { bill::statBill(); });
}

void MainWindow::setWindowBill() { ui->tableViewBill->setModel(Bi.read()); }
void MainWindow::setwindowHome() {

  // Chat

  connect(ui->pushButtonchat, &QPushButton::clicked, this, [this]() {
    QString prompt = ui->lineEditchat->text();
    QString model = "davinci";
    QString api_key = "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";
    assistant::generate_and_display_text(prompt, model, api_key,
                                         ui->plainTextEditchat);
  });

  // Weather

  const auto updateWeather = [this](const Weather &w) {
    ui->weatherLocation->setText(w.location);
    ui->lineEditFeelslikeWeather->setText(QString::number(w.feelsLike));
    ui->lineEditHumidityWeather->setText(QString::number(w.humidity));
    ui->lineEditTempMaxWeather->setText(QString::number(w.tempMax));
    ui->lineEditLevelWeather->setText(QString::number(w.gnd));
    ui->lineEditNormalTempWeather->setText(QString::number(w.temperature));
    ui->lineEditPressureWeather->setText(QString::number(w.pressure));
    ui->lineEditTempMinWeather->setText(QString::number(w.tempMin));
    ui->lineEditSeaLevelWeather->setText(QString::number(w.sea));
  };

  QTimer *weatherTimer = new QTimer(this);

  if (QGeoPositionInfoSource *source =
          QGeoPositionInfoSource::createDefaultSource(this);
      source) {
    ui->weatherButton->setDisabled(true);
    QGeoCoordinate *coordinates = new QGeoCoordinate(0, 0);
    source->setUpdateInterval(120'000); // every 2 minutes
    source->startUpdates();

    connect(source,
            QOverload<QGeoPositionInfoSource::Error>::of(
                &QGeoPositionInfoSource::error),
            [=](QGeoPositionInfoSource::Error positioningError) {
              qDebug() << positioningError;
            });

    connect(source, &QGeoPositionInfoSource::positionUpdated,
            [=](const QGeoPositionInfo &info) {
              if (*coordinates == QGeoCoordinate(0, 0)) {

                if (!coordinates->isValid())
                  return;

                // set the coordinates

                coordinates->setLatitude(info.coordinate().latitude());
                coordinates->setLongitude(info.coordinate().longitude());

                emit coordsMadeAvailable(info);
              } else
                *coordinates = info.coordinate();
            });

    // Once the location is initially made available, activate the weather
    // update timer
    connect(this, &MainWindow::coordsMadeAvailable, this,
            [=](const QGeoPositionInfo &info) {
              // for every next coordinate
              connect(weatherTimer, &QTimer::timeout, this, [=]() {
                if (coordinates == nullptr || !coordinates->isValid())
                  return;
                Weather::getInfos(*coordinates, updateWeather);
              });

              weatherTimer->start(10'000); // every ten seconds

              // first time
              Weather::getInfos(info.coordinate(), updateWeather);
            });

    // Start an initial location update
    source->requestUpdate();

  } else {
    ui->weatherLocation->setPlaceholderText("Please set your location");
    // query the location from the user on click
    connect(ui->weatherButton, &QPushButton::released, this, [this, updateWeather]() {
      const QString location = QInputDialog::getText(this, "Choose a weather location",
                                           tr("City"), QLineEdit::Normal)
                         .simplified();
      if ( location.isEmpty() )
        return;

      // first time
      if (weatherLocation.isNull()) {
        emit locationMadeAvailable(weatherLocation);
      }

      // set the location
      weatherLocation = location;

      // update immediatly
      Weather::getInfos(weatherLocation, updateWeather);
    });

    // start the timer once the location is initially made available
    connect(this, &MainWindow::locationMadeAvailable, this,
            [weatherTimer, updateWeather, this]() {
              connect(weatherTimer, &QTimer::timeout, this, [=]() {
                Weather::getInfos(weatherLocation, updateWeather);
              });
              weatherTimer->start(10'000);
            });

    qDebug() << "No geolocation source";
  }
}
void MainWindow::staffSetup() {
  staffW = new StaffWidget(this);
  ui->StaffPage->layout()->addWidget(staffW);
}

// Arduino

void MainWindow::arduinoSetup() {

  // Arduino

  Arduino *rfid = nullptr, *trafficLights = nullptr, *heartMonitor = nullptr;

  if (const auto info = Arduino::find(Arduino::Login); info.has_value()) {
    rfid = new Arduino(info.value());

    // CHECK
    connect(
        rfid->serial(), &QIODevice::readyRead, rfid->serial(), [rfid, this]() {
          // begin
          if ((QDateTime::currentSecsSinceEpoch() - lastScanTime) < 1)
            return;
          QString uid = rfid->readline(); // read the UID from the Arduino
          //    lastScanTime = QDateTime::currentSecsSinceEpoch();

          qDebug() << uid;
          if (uid.isEmpty())
            return;

          lastScanTime = QDateTime::currentSecsSinceEpoch();

          qDebug() << "UID: " << uid.toInt();
          // int uid1 = uid.toInt();

          QSqlQuery query;
          query.prepare("SELECT FNAME, LNAME, ROLE FROM STAFF WHERE S_ID =" +
                        uid);
          // query.bindValue(0, uid1);
          // query.addBindValue(uid1);

          if (!query.exec()) {
            qDebug() << " " << query.lastError().text();
            // RFID not recognized by the database

            ui->uidLabel->setText("UID: " + uid + " (RFID not recognized)");
            ui->firstNameLabel->setText("");
            ui->timeLabel->setText("");
            ui->statusLabel->setText("");
            ui->messageLabel->setText("");

          } else {
            query.first();

            const QString firstName = query.value("FNAME").toString(),
                          lastName = query.value("LNAME").toString(),
                          role = query.value("ROLE").toString();

            // Check if the employee is a administrator

            QByteArray datum;
            if (role.toLower() == "administrator") {

              // Write 1 to Arduino
              datum = "0";
              rfid->write(datum);
            } else {
              datum = "1";
              rfid->write(datum);
            }
            qDebug() << datum;

            qDebug() << QString("Welcome, %1 %2").arg(firstName, lastName);

            QDateTime currentDateTime = QDateTime::currentDateTime();

            // Convert the date and time to a string
            QString currentDateTimeString =
                currentDateTime.toString("yyyy-MM-dd hh:mm:ss");

            ui->uidLabel->setText("UID: " + uid);
            ui->firstNameLabel->setText("Welcome " + firstName + "  " +
                                        lastName + " !");
            ui->timeLabel->setText("Time of arrival: " + currentDateTimeString);
            ui->statusLabel->setText("Employee Status: " + role);

            if (currentDateTime.time().hour() >= 8) {
              // Calculate the number of minutes late
              int minutesLate = currentDateTime.time().minute() +
                                (currentDateTime.time().hour() - 8) * 60;
              QString lateMessage =
                  QString("YOU ARE %1 MINUTES LATE!").arg(minutesLate);
              ui->messageLabel->setText(lateMessage);
            } else {
              // Calculate the number of minutes early
              int minutesEarly = (8 - currentDateTime.time().hour()) * 60 -
                                 currentDateTime.time().minute();
              QString earlyMessage =
                  QString("YOU ARE %1 MINUTES EARLY!").arg(minutesEarly);
              ui->messageLabel->setText(earlyMessage);
            }
          }
        });

    rfid->connect();
  }

  if (const auto info = Arduino::find(Arduino::HeartMonitor);
      info.has_value()) {
    heartMonitor = new Arduino(info.value());
    connect(heartMonitor->serial(), &QIODevice::readyRead,
            heartMonitor->serial(), [heartMonitor, this]() {
              // read heart pulse value from Arduino for 30 seconds and
              // calculate the average value

              QElapsedTimer timer;
              timer.start();
              int heart_pulse = QString(heartMonitor->readline()).toInt();

              // insert average heart pulse value into database
              QSqlQuery query;
              query.prepare("UPDATE heart_pulses SET pulses=:n WHERE ID_P=88");
              query.bindValue(":n", heart_pulse);
              qDebug() << heart_pulse;
              if (!query.exec()) {
                qDebug() << "Failed to insert heart pulse into database";
                return;
              }

              // close connection to Arduino
            });

    heartMonitor->connect();
  }

  if (const auto info = Arduino::find(Arduino::Traffic); info.has_value()) {
    trafficLights = new Arduino(info.value());

    connect(trafficLights->serial(), &QIODevice::readyRead,
            trafficLights->serial(), [trafficLights, this]() {
              dataTraffic = trafficLights->read();

              bool test = trafficLights->updateScenario(dataTraffic.toInt());
              if (test)
                qDebug() << "success";
              else
                qDebug() << " NO success";

              int read = trafficLights->getpos();
              qDebug() << read;
              switch (read) {
              case 1:
                trafficLights->write("1");

                break;
              case 2:
                trafficLights->write("2");
                break;
              case 3:
                trafficLights->write("3");

                break;
              case 4:
                trafficLights->write("4");

                break;
              }
            });

    trafficLights->connect();
  }

  // end arduino
}

void MainWindow::getMapImage() {
  QString apiKey =
      "fMMo88whlGnaY4fBOUhW~ER47Ons-jnYFPFZPwm2UQg~AgnRTqvOXh0b9yCe3Rrxv-"
      "2o07W3PkZJkMzm3NbKQlswXNy0gYraV5K2tAivstJE";
  QString query = ui->lineEdit->text();
  QString mapSize = QString::number(1300) + "," + QString::number(300);
  int zoomLevel = 14;

  QUrl url("https://dev.virtualearth.net/REST/v1/Imagery/Map/Road/" +
           QUrl::toPercentEncoding(query) + "/" + QString::number(zoomLevel) +
           "?mapSize=" + mapSize + "&key=" + apiKey);

  qDebug() << "Request URL:" << url.toString();

  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  QNetworkReply *reply = manager->get(QNetworkRequest(url));

  connect(reply, &QNetworkReply::finished, [=]() {
    if (reply->error() == QNetworkReply::NoError) {
      QPixmap mapImage;
      mapImage.loadFromData(reply->readAll());
      ui->labelmap->setPixmap(mapImage);
    } else {
      qDebug() << "Error:" << reply->errorString();
    }

    reply->deleteLater();
    manager->deleteLater();
  });
}

void MainWindow::transcriptSetup() {
  AudioTranscript *transcripter =
      new AudioTranscript(AudioTranscript::Api::WebSocket, this);

  connect(ui->transcriptStartButton, &QPushButton::released, transcripter,
          &AudioTranscript::startRecording);
  connect(ui->transcriptStopButton, &QPushButton::released, transcripter,
          &AudioTranscript::stopRecording);

  // For websocket

  connect(transcripter, &AudioTranscript::receivedFromSocket, this,
          [this, transcripter](const QString &message) {
            const QString text = transcripter->fromResponse(message.toUtf8());
            if (text.isEmpty())
              return;

            // Append to the form

            ui->transcriptResult->append(text);
          });

  // For requests

  connect(transcripter, &AudioTranscript::recordedSession, this,
          [this, transcripter](const QByteArray &audio) {
            AudioTranscript::retrieveTranscriptFromWav(
                AudioTranscript::rawToWav(audio, transcripter->format()),
                [this, transcripter](QNetworkReply *reply) {
                  const QByteArray resp = reply->readAll();

                  const QString text = transcripter->fromResponse(resp);

                  ui->transcriptResult->setText(text);
                });
          });
}
