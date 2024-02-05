#include "staffwidget.h"
#include "ui_staffwidget.h"

StaffWidget::StaffWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::StaffWidget) {
  ui->setupUi(this);

  // statistics button

  connect(ui->statisticsButton, &QPushButton::released, this,
          &StaffWidget::showStats);

  // card button

  connect(ui->cardButton, &QPushButton::released, this,
          &StaffWidget::showEmployeeCard);

  // staff page

  ui->listView->setModel(model);

  // so that selecting a model index behaves as expected

  ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);

  // ui->scrollArea->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  // preserve position when hidden
  QSizePolicy spFormWidget = ui->formWidget->sizePolicy();
  spFormWidget.setRetainSizeWhenHidden(true);
  ui->formWidget->setSizePolicy(spFormWidget);

  // preserve position when hidden
  QSizePolicy spAccountFormWidget = ui->accountFormWidget->sizePolicy();
  spAccountFormWidget.setRetainSizeWhenHidden(true);
  ui->accountFormWidget->setSizePolicy(spAccountFormWidget);

  // start hidden

  ui->accountFormWidget->hide();

  ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();

  // set property of account creation button
  ui->accountCreationButton->setText("add");

  // Input control
  ui->nationalIdField->setValidator(new QIntValidator(0, 100000));
  for (auto nameField : {ui->lastNameField, ui->firstNameField})
    nameField->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z' ]*")));

  // hide password characters
  ui->passwordField->setEchoMode(QLineEdit::Password);

  // password visibility
  const QAction *togglePasswordVisibility = ui->passwordField->addAction(
      style()->standardIcon(QStyle::SP_TitleBarContextHelpButton),
      QLineEdit::TrailingPosition);

  connect(togglePasswordVisibility, &QAction::triggered, ui->passwordField,
          [this]() {
            ui->passwordField->setEchoMode(ui->passwordField->echoMode() ==
                                                   QLineEdit::Normal
                                               ? QLineEdit::Password
                                               : QLineEdit::Normal);
          });

  // password regeneration action
  const QAction *passwordGeneration = ui->passwordField->addAction(
      style()->standardIcon(QStyle::SP_BrowserReload),
      QLineEdit::TrailingPosition);

  connect(passwordGeneration, &QAction::triggered, ui->passwordField, [this]() {
    ui->passwordField->setText(Account::generatePassword());
    ui->passwordField->setEchoMode(QLineEdit::Normal);
  });

  // username generation

  for (auto nameField : {ui->lastNameField, ui->firstNameField}) {
    connect(nameField, &QLineEdit::textEdited, nameField, [this]() {
      // ensure that account creation is active

      if (ui->accountCreationButton->text() == "add" ||
          ui->accountFormWidget->isHidden())
        return;

      // Don't regenerate if editing an existing member who has a username
      // TODO: implement

      ui->usernameField->setText(Account::generateUsername(
          ui->firstNameField->text(), ui->lastNameField->text()));
    });
  }

  // connect bottom right actions

  // TODO: implement

  connect(ui->buttonBox, &QDialogButtonBox::clicked, this,
          [this](QAbstractButton *button) {
            switch (ui->buttonBox->standardButton(button)) {
            case QDialogButtonBox::Discard:

              if (mode == FormMode::NewMember)
                clearForm();
              else if (auto member = currentySelectedListMember();
                       member.has_value()) {
                loadMemberOnForm(member.value());
              }
              break;
            case QDialogButtonBox::Save: {
              const Staff member = memberFromForm();

              if (mode == FormMode::NewMember) {
                // NOTE: maybe check for error?

                const auto id = member.create();
                if (!id.has_value()) {
                  // insertion failed
                } else {
                  // refresh
                  // model->setQuery(model->query().lastQuery());
                  loadMembersList(id.value());
                }
              } else {

                // If editing

                if (!member.update()) {
                  // error, update failed
                } else {
                  // refresh
                  // model->setQuery(model->query().lastQuery());
                  loadMembersList(member.getId());
                }
              }

              break;
            }
            case QDialogButtonBox::Cancel: {
              if (mode == FormMode::Editing)
                return;
              break;
            }
            default:
              break;
            }
          });

  // edit

  // on double click

  // connect(ui->listView, &QListView::activated, this,
  //         [this](const QModelIndex &index) {
  //           if (const auto selectedMember = currentySelectedMember();
  //               selectedMember.has_value()) {
  //             loadMemberOnForm(selectedMember.value());
  //           }
  //           mode = FormMode::Editing;

  //           // Ensure form is visible

  //           if (ui->formWidget->isHidden())
  //             ui->formWidget->show();

  //           // no cancel

  //           if (ui->buttonBox->button(QDialogButtonBox::Cancel)->isVisible())
  //             ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();
  //         });

  // on click

  connect(
      ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged,
      model,
      [this](const QItemSelection &selected, const QItemSelection &deselected) {
        if (const auto selectedMember = currentySelectedListMember();
            selectedMember.has_value()) {
          loadMemberOnForm(selectedMember.value());
        }
        mode = FormMode::Editing;

        // Ensure form is visible

        if (ui->formWidget->isHidden())
          ui->formWidget->show();

        // no cancel

        if (ui->buttonBox->button(QDialogButtonBox::Cancel)->isVisible())
          ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();
      });

  // new member button

  connect(ui->newBtn, &QPushButton::released, this, [this]() {
    // ignore if already creating a new member

    if (mode == FormMode::NewMember)
      return;

    // unhide form if hidden
    if (ui->formWidget->isHidden())
      ui->formWidget->setVisible(true);

    ui->buttonBox->button(QDialogButtonBox::Cancel)->show();

    mode = FormMode::NewMember;

    // Show new member if added to list
    // TODO: implement

    clearForm();
  });

  // delete

  connect(ui->deleteBtn, &QPushButton::released, this, [this]() {
    const auto currentMember = currentySelectedListMember();

    // If no member is selected
    if (!currentMember.has_value())
      return;

    // Delete
    if (!currentMember.value().remove()) {
      // Failure to execute
      return;
      // TODO: log some error
    }

    // refresh

    loadMembersList();

    // Id of the member to delete  (currently selected)

    // const int buttonId = 0;

    // if (buttonId == 0 || buttonId == -1)
    //   return; // Ignore for 'new member' button or if no button is active

    // Remove the staff member from the list

    // Clear the history stack from all references to the id to delete

    // NOTE: unnecessarily expensive operation?

    // buttonHistory.removeAll(buttonId);

    // Remove the staff member from the database

    // Staff::remove(buttonId);

    // post deletion

    // Hide form when all are gone

    if (!ui->listView->currentIndex().isValid())
      ui->accountFormWidget->hide();
  });

  // search

  connect(ui->searchBox, &QLineEdit::textEdited, this,
          [this](const QString &text) {
            const auto currMember = currentySelectedListMember();
            loadMembersList(currMember.has_value() ? currMember->getId() : -1);
          });

  // sort

  connect(ui->dirBtn, &QPushButton::released, this, [this]() {
    sortDirection = (sortDirection == Qt::DescendingOrder)
                        ? Qt::AscendingOrder
                        : Qt::DescendingOrder;
    // model->sort(0, sortDirection);
    const auto currMember = currentySelectedListMember();
    loadMembersList(currMember.has_value() ? currMember->getId() : -1);
  });

  connect(ui->accountCreationButton, &QPushButton::released, this, [this]() {
    const bool adding = ui->accountCreationButton->text() == tr("add");

    ui->accountCreationButton->setText(tr(adding ? "remove" : "add")); // toggle

    // Toggle visibility
    ui->accountFormWidget->setVisible(adding);

    if (adding) {
      // Don't generate if editing an existing member who has an account

      if (const auto member = currentySelectedListMember();
          mode == FormMode::Editing && member.has_value() &&
          member.value().account.has_value()) {

        ui->usernameField->setText(
            member.value().account.value().getUserName());

      } else if (const QString first = ui->firstNameField->text(),
                 last = ui->lastNameField->text();
                 !(first).isEmpty() && !last.isEmpty()) {

        ui->usernameField->setText(Account::generateUsername(first, last));

        // generate and display the password accordingly
        ui->passwordField->setText(Account::generatePassword());
        ui->passwordField->setEchoMode(QLineEdit::Normal);
      }
    } else {
      // if(!ui->usernameField->text().isEmpty())
      ui->usernameField->setText({});
      // if(!ui->passwordField->text().isEmpty())
      ui->passwordField->setText({});
    }

    // ui->delAccountButton->show();
  });

  // Google Drive Oauth

  // TODO: instead of disabling this button,
  // the upload button should be disabled
  ui->cardButton->setEnabled(false);

  oauthClient = new GoogleDriveOauth;

  oauthClient->setup();

  // XXX:
  connect(oauthClient, &GoogleDriveOauth::accessUpdate, ui->cardButton,
          &QPushButton::setEnabled);

  if (oauthClient->google != nullptr) {
    // TODO: this should only be invoked when needed, not on initialization

    oauthClient->google->refreshAccessToken();
  }
}

StaffWidget::~StaffWidget() { delete ui; }

void StaffWidget::loadMembersList(int chosenId) {
  model->setQuery(Staff::findAll(searchQuery(), Staff::SearchCriteria::FullName,
                                 sortDirection));
  // set index to first by default
  if (chosenId == -1) {
    ui->listView->setCurrentIndex(model->index(0, 0));

    return;
  }

  if (const QModelIndexList list =
          model->match(model->index(0, model->record().indexOf("S_ID")),
                       Qt::DisplayRole, chosenId, 1, Qt::MatchExactly);
      !list.isEmpty()) {

    // qDebug() << "found"
    //          << (dynamic_cast<const QSqlQueryModel
    //          *>(list.constFirst().model())
    //                  ->query()
    //                  .value("S_ID"));

    // qDebug() << model->data(list.constFirst(), Qt::DisplayRole).toString();

    ui->listView->setCurrentIndex(list.constFirst());
  } else
    ui->listView->setCurrentIndex(model->index(0, 0));

  // ui->listView->selectionModel()->select(
  //     model->index(1, 0), QItemSelectionModel::SelectionFlag::Select);
}

void StaffWidget::loadMemberOnForm(const Staff &member) {

  for (const auto &[field, value] : std::unordered_map<QString, QString>{
           {"nationalIdField",
            member.getNationalId().has_value()
                ? QString::number(member.getNationalId().value())
                : QString()},
           {"idField",
            member.getId() != 0 ? QString::number(member.getId()) : QString()},
           {"firstNameField", member.getFirstName()},
           {"lastNameField", member.getLastName()},
       })
    ui->formWidget->findChild<QLineEdit *>(field)->setText(value);

  for (const auto &[field, value] : std::unordered_map<QString, QString>{
           {"roleField", member.getRole()},
           {"statusField", member.getStatus()},
           {"genderField", member.getGender()},
       })
    ui->formWidget->findChild<QComboBox *>(field)->setCurrentText(value);

  if (member.account.has_value()) {
    // ui->accountCreationButton->hide();

    ui->accountCreationButton->setText(tr("remove"));
    ui->usernameField->setText(member.account.value().getUserName());

    ui->accountFormWidget->show();

    ui->passwordField->setText(QString());

    // password remains empty

    // ui->accountCreationButton->setProperty("usage", "remove");
    // ui->delAccountButton->show();
  } else {

    // ui->accountCreationButton->setProperty("usage", "add");

    ui->accountCreationButton->setText("add");

    // if(ui->delAccountButton->isVisible())
    // ui->delAccountButton->hide();

    // if(!ui->usernameField->text().isEmpty())
    ui->usernameField->setText({});

    // if(!ui->passwordField->text().isEmpty())
    ui->passwordField->setText({});

    // if (ui->accountFormWidget->isVisible())
    ui->accountFormWidget->hide();

    // ui->accountFormLayout->children().at(0)->children().at(0).hide

    // if (ui->accountCreationButton->isHidden())
    // ui->accountCreationButton->show();
  }
}

void StaffWidget::clearForm() {
  // TODO: implement
  loadMemberOnForm({});
}

std::optional<Staff> StaffWidget::currentySelectedListMember() const {

  const QModelIndex currentIndex = ui->listView->currentIndex();
  if (!currentIndex.isValid()) {
    return std::nullopt;
  }
  const int row = currentIndex.row();

  return Staff(model->record(row));
}

Staff StaffWidget::memberFromForm() const {

  std::optional<Account> account = std::nullopt;

  // Creating a user

  if (!ui->usernameField->text().isEmpty()) {
    account.emplace();
    account.value().setUserName(ui->usernameField->text());
    auto placeholder = currentySelectedListMember();

    // hash password from field
    if (!ui->passwordField->text().isEmpty()) {
      // If not empty then store it
      account.value().setPassword(
          Account::hashPassword(ui->passwordField->text()));
    } else if (auto member = currentySelectedListMember();
               member.has_value() && member->hasAccount()) {
      account.value().setPassword(member->account->getPassword());
    } else {
      // Error;
    }
  }

  return Staff(
      ui->idField->text().toInt(),
      ui->nationalIdField->text().isEmpty()
          ? std::nullopt
          : std::optional<unsigned int>{ui->nationalIdField->text().toInt()},
      ui->firstNameField->text().simplified(),
      ui->lastNameField->text().simplified(), ui->genderField->currentText(),
      ui->roleField->currentText(), ui->statusField->currentText(), QString(),
      account);
}

void StaffWidget::showStats() const {

  QMainWindow *statWindow = new QMainWindow();
  QWidget *center = new QWidget();
  QGridLayout *lay = new QGridLayout(center);
  center->setLayout(lay);
  statWindow->setCentralWidget(center);

  // Show these three charts

  for (Statistics categ :
       {Statistics::Account, Statistics::Role, Statistics::Gender})
    lay->addWidget(genChartView(categ));

  // Force constant size floating window

  statWindow->setFixedSize(
      QGuiApplication::primaryScreen()->geometry().width() / 4,
      2 * QGuiApplication::primaryScreen()->geometry().height() / 3);
  statWindow->show();
}

QChartView *StaffWidget::genChartView(Statistics category) const {

  // const QHash<QString, QString> criteriaToLabel{
  //     {"Male", "Men"},
  //     {"Female", "Women"},
  //     {"Paramedic", "Paramedics"},
  //     {"Maintenance", "Maintenance"},
  //     {"Administrator", "Administrator"},
  // };

  QPieSeries *series = new QtCharts::QPieSeries();

  QChart *chart = new QChart();
  chart->addSeries(series);
  // chart->legend()->hide();

  series->setLabelsVisible();
  series->setLabelsPosition(QPieSlice::LabelInsideHorizontal);

  switch (category) {
  case Statistics::Gender: {
    const unsigned int nbrWomen = Staff::totals("gender", "Female"),
                       nbrMen = Staff::totals("gender", "Male");
    QPieSlice *men = new QPieSlice(tr("Men"), nbrMen),
              *women = new QPieSlice(tr("Women"), nbrWomen);
    men->setBrush(QColorConstants::Svg::lightskyblue);
    women->setBrush(QColorConstants::Svg::lightpink);
    series->append({men, women});

    chart->setTitle(tr("Gender distribution"));
    break;
  }
  case Statistics::Role: {
    const unsigned int nbrParamedics = Staff::totals("role", "Paramedic"),
                       nbrAdmins = Staff::totals("role", "Administrator");
    QPieSlice *admins = new QPieSlice(tr("Administrators"), nbrAdmins),
              *paramedics = new QPieSlice(tr("Paramedics"), nbrParamedics);
    admins->setBrush(QColorConstants::Svg::mediumturquoise);
    paramedics->setBrush(QColorConstants::Svg::tomato);
    series->append({admins, paramedics});

    chart->setTitle(tr("Staff Roles"));
    break;
  }
  case Statistics::Account: {
    const unsigned int nbrAccounts = Staff::totals("has_account", 1),
                       nbrNoAccounts = Staff::totals("has_account", 0);

    QPieSlice *hasAccount = new QPieSlice(tr("Accounts"), nbrAccounts),
              *noAccount = new QPieSlice(tr("No accounts"), nbrNoAccounts);
    hasAccount->setBrush(QColorConstants::Svg::greenyellow);
    noAccount->setBrush(QColorConstants::Svg::crimson);
    series->append({hasAccount, noAccount});

    chart->setTitle(tr("Proportion of account owners"));
    break;
  }
  }

  foreach (auto slice, series->slices()) {
    // slice->setExploded();
    slice->setLabelVisible();
    slice->setPen(QPen(Qt::darkGreen, 2));
    // slice->setLabel(QString("%1%").arg(100 * slice->percentage(), 0, 'f',
    // 1));
  }
  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  return chartView;
}

void StaffWidget::showEmployeeCard() const {

  const auto currMember = currentySelectedListMember();

  if (!currMember.has_value())
    return;

  // create a QPainter to render the document

  QTextDocument document(this->parentWidget());
  document.setHtml(currMember->renderCardTemplate());

  QImage *htmlImage = new QImage(165, 200, QImage::Format_ARGB32_Premultiplied);
  htmlImage->fill(Qt::white);
  QPainter htmlPainter(htmlImage);
  document.drawContents(&htmlPainter);

  // create a QWidget to display the image

  QWidget *previewWidget = new QWidget();
  previewWidget->setWindowIcon(QIcon(":/rsc/img/ambulance_trans.png"));
  previewWidget->setWindowTitle("Staff member " +
                                QString::number(currMember->getId()));
  previewWidget->setWindowIcon(QIcon(":/rsc/img/user.svg"));
  // previewWidget->setGeometry(0, 0, 400, 250);
  // previewWidget->setFixedSize(previewWidget->size());
  // previewWidget->setFixedSize(150, 200);
  previewWidget->setFixedSize(180, 280);

  // layout
  new QVBoxLayout(previewWidget);

  QWidget *previewPic = new QWidget;

  QLabel *label = new QLabel(previewPic);
  label->setPixmap(QPixmap::fromImage(*htmlImage));
  label->setAlignment(Qt::AlignCenter);

  previewWidget->layout()->addWidget(previewPic);

  QPushButton *const saveButton = new QPushButton(QIcon(":/rsc/img/save.svg"),
                                                  tr("Save"), previewWidget),
                     *const uploadButton =
                         new QPushButton(QIcon(":/rsc/img/upload-cloud.svg"),
                                         tr("Upload"), previewWidget);

  previewWidget->setStyleSheet(R"(QPushButton {
          color: #fff;
          background-color: rgb(31, 35, 42);
          border-radius: 10px;
          padding: 5px 10px;
          text-align: center;
          border: none;
          })");

  connect(
      saveButton, &QPushButton::released, previewWidget,
      [label, currMember, previewWidget]() {
        const QString defaultName =
            tr("employeeCard") + QString::number(currMember->getId()) + ".png";

        const QString fileName = QFileDialog::getSaveFileName(
            previewWidget, tr("Save Image"), qgetenv("HOME") + '/' + defaultName
            // , tr("Image Files (*.png *.jpg *.bmp)")
        );
        if (fileName.isEmpty())
          return;

        label->pixmap(Qt::ReturnByValue).save(fileName);
      });

  connect(uploadButton, &QPushButton::released, previewWidget,
          [this, label, currMember]() {
            const QString targetName = tr("employeeCard") +
                                       QString::number(currMember->getId()) +
                                       ".png";

            QBuffer *buffer = new QBuffer;
            buffer->open(QIODevice::WriteOnly);
            label->pixmap(Qt::ReturnByValue).save(buffer, "PNG");
            buffer->close();
            oauthClient->upload(buffer, targetName, "image/png",
                                oauthClient->google->token());
          });

  previewWidget->layout()->addWidget(saveButton);
  previewWidget->layout()->addWidget(uploadButton);

  // show the preview widget

  previewWidget->show();
}
