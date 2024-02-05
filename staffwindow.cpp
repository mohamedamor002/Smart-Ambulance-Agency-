#include "staffwindow.h"
#include "ui_staffwindow.h"

using std::find_if, std::remove_if, std::replace_if;

StaffWindow::StaffWindow(QWidget *parent, const Staff &member)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);


  // statistics button

  connect(ui->statisticsButton, &QPushButton::released, this,
          [this]() { showStats(); });

  // card button

  connect(ui->cardButton, &QPushButton::released, this,
          [this]() { showEmployeeCard(); });

  // staff page

  ui->scrollArea->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  ui->verticalLayout->setAlignment(Qt::AlignTop);

  // preserve position when hidden
  QSizePolicy spFormWidget = ui->formWidget->sizePolicy();
  spFormWidget.setRetainSizeWhenHidden(true);
  ui->formWidget->setSizePolicy(spFormWidget);

  // preserve position when hidden
  QSizePolicy spAccountFormWidget = ui->accountFormWidget->sizePolicy();
  spAccountFormWidget.setRetainSizeWhenHidden(true);
  ui->accountFormWidget->setSizePolicy(spAccountFormWidget);

  // ui->formWidget->hide();

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

  // username generation action

  // const QAction *generateUsername = ui->passwordField->addAction(
  //     style()->standardIcon(QStyle::SP_BrowserReload),
  //     QLineEdit::TrailingPosition);

  // connect(generateUsername, &QAction::triggered, ui->passwordField, [this]()
  // {
  //   const Staff member = memberFromForm();
  //   if (!member.getFirstName().isEmpty() && !member.getLastName().isEmpty())
  //   {
  //     ui->usernameField->setText(Account::generateUsername(
  //         member.getFirstName(), member.getLastName()));
  //   }
  // });

  for (auto nameField : {ui->lastNameField, ui->firstNameField}) {
    connect(nameField, &QLineEdit::textEdited, nameField, [this]() {
      // ensure that account creation is active

      if (ui->accountCreationButton->text() == "add" ||
          ui->accountFormWidget->isHidden())
        return;

      // Don't regenerate if editing an existing member who has a username
      if (buttonGroup->checkedId() != 0 &&
          findMember(buttonGroup->checkedId()).account.has_value()) {
        return;
      }

      ui->usernameField->setText(Account::generateUsername(
          ui->firstNameField->text(), ui->lastNameField->text()));
    });
  }

  // button for new member

  addButton("New Member", "staffMemberNew", 0, false);

  connect(
      buttonGroup, &QButtonGroup::idPressed, buttonGroup, [this](int buttonId) {
        // No need to retrigger event if button is already pressed
        if (buttonId == buttonGroup->checkedId())
          return;

        // If the newly clicked button is 'new member'
        if (buttonId == 0) {
          // buttonHistory.push(buttonId);
          selectMember(newMember);
          ui->buttonBox->button(QDialogButtonBox::Cancel)->show();
          return;
        }

        // Previous button was `new member`
        if (buttonGroup->checkedId() == 0) {
          ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();
          // save member draft, (except for the case of an insertion?)
          newMember = memberFromForm();
        } else
          buttonHistory.push(buttonId); // add the button to the history stack,
                                        // only if not the 'new member button'

        // load the staff member info on the form
        selectMember(buttonId);
      });

  connect(ui->deleteBtn, &QPushButton::released, this, [this]() {
    // Id of the member to delete  (currently selected)

    const int buttonId = buttonGroup->checkedId();

    if (buttonId == 0 || buttonId == -1)
      return; // Ignore for 'new member' button or if no button is active

    // Remove the button widget and unlink it from its group and layout

    removeButton(buttonGroup->checkedButton());

    // Remove the staff member from the Vector

    staffMembers.erase(remove_if(
        staffMembers.begin(), staffMembers.end(), [buttonId](Staff member) {
          return member.getId() == (unsigned int)buttonId;
        }));

    // Clear the history stack from all references to the id to delete

    // NOTE: unnecessarily expensive operation?

    buttonHistory.removeAll(buttonId);

    // Remove the staff member from the database

    Staff::remove(buttonId);

    // post deletion

    const QList<QAbstractButton *> &buttons = buttonGroup->buttons();

    // exception
    if (buttons.empty()) {
      qWarning() << "There are zero member buttons";
      return;
    }

    // no members  except for 'new member', which is not selected
    if (buttons.length() == 1) {

      // exception
      if (buttonGroup->id(buttons.constFirst()) != 0) {
        qWarning() << "First button is not 'new member' button";
        return;
      }

      // clear the form

      selectMember();

      ui->formWidget->hide();

      return;
    }

    // && buttonGroup->button(buttonHistory.top()) != nullptr
    ((!buttonHistory.empty()) ? buttonGroup->button(buttonHistory.top())
                              : buttons.at(1))
        ->click();
    // NOTE: verify when to use animateClick()
  });

  connect(ui->newBtn, &QPushButton::released, this, [this]() {
    // ignore if already selected
    if (buttonGroup->checkedId() == 0 && buttonGroup->button(0)->isVisible())
      return;

    // unhide form if hidden
    if (ui->formWidget->isHidden())
      ui->formWidget->setVisible(true);

    const auto &newMemberButton = buttonGroup->button(0);
    if (newMemberButton->isHidden())
      newMemberButton->show();

    newMemberButton->click();
  });

  connect(ui->buttonBox, &QDialogButtonBox::clicked, this,
          [this](QAbstractButton *button) {
            const int buttonId = buttonGroup->checkedId();

            switch (ui->buttonBox->standardButton(button)) {
            case QDialogButtonBox::Discard:
              selectMember(buttonId);
              break;
            case QDialogButtonBox::Save: {
              if (buttonId == -1)
                return;

              Staff formMember = memberFromForm();
              // New member button: create new member

              if (buttonId == 0) {

                if (!validateSubmission(formMember, Submission::Creation)) {
                  return;
                }

                const auto newlyCreatedMemberId = formMember.create();

                // Creation failed

                if (!newlyCreatedMemberId.has_value())
                  return;

                formMember.setId(newlyCreatedMemberId.value());
                addButton(formMember.getFirstName() + ' ' +
                              formMember.getLastName(),
                          "staffMember" + QString::number(formMember.getId()),
                          formMember.getId());

                buttonGroup->button(0)->hide();
                newMember = {};
                selectMember();

                staffMembers.append(formMember);

                buttonGroup->button(newlyCreatedMemberId.value())
                    ->click(); // will retrigger
                return;
              }

              // Else, update an existing member

              if (!validateSubmission(formMember, Submission::Update)) {
                return;
              }

              if (!formMember.update()) {
                return;
              }

              // change the stored member in the vector

              replace_if(
                  staffMembers.begin(), staffMembers.end(),
                  [buttonId](Staff member) {
                    return (int)member.getId() == buttonId;
                  },
                  formMember);

              // update the button name

              buttonGroup->button(buttonId)->setText(
                  formMember.getFirstName() + ' ' + formMember.getLastName());

              break;
            }
            case QDialogButtonBox::Cancel: {
              if (buttonId != 0)
                return;
              selectMember();                 // empty the form
              buttonGroup->button(0)->hide(); // hide 'new member' button

              // member button to click

              // if there are no members to select, simply hide the form
              if (staffMembers.empty() ||
                  buttonGroup->buttons().length() <= 1) {
                ui->formWidget->hide();
                return;
              }

              if (buttonHistory.empty()) {
                // if there is no history, then click the first member
                buttonGroup->button(staffMembers.constFirst().getId())->click();
              } else if (buttonGroup->button(buttonHistory.top())) {
                // if there is history and the corresponding last button exists
                buttonGroup->button(buttonHistory.top())->click();
              } else {
                // exception
                qWarning() << (QString() + "Button " + buttonHistory.top() +
                               " not found in history");
                buttonGroup->button(staffMembers.constFirst().getId())
                    ->click(); // reselect the last member
              }

              break;
            }
            default:
              break;
            }
          });

  connect(ui->searchBox, &QLineEdit::textEdited, this,
          [this](const QString &text) {
            // qDebug() << text;
            searchQuery = text;
            loadMembersList();
          });

  connect(ui->dirBtn, &QPushButton::released, this, [this]() {
    sortDirection = (sortDirection == Qt::DescendingOrder)
                        ? Qt::AscendingOrder
                        : Qt::DescendingOrder;
    loadMembersList(!buttonHistory.empty() ? buttonHistory.top() : -1);
  });

  connect(ui->accountCreationButton, &QPushButton::released, this, [this]() {
    const bool adding = ui->accountCreationButton->text() == tr("add");

    ui->accountCreationButton->setText(tr(adding ? "remove" : "add")); // toggle

    // Toggle visibility
    ui->accountFormWidget->setVisible(adding);

    if (adding) {
      // Don't generate if editing an existing member who has an account

      if (const Staff member = findMember(buttonGroup->checkedId());
          buttonGroup->checkedId() != 0 && member.account.has_value()) {

        ui->usernameField->setText(member.account.value().getUserName());

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

  // Initial loading

  loadMembersList();

  // Google Drive Oauth

  // TODO: instead of disabling this button,
  // the upload button should be disabled
  ui->cardButton->setEnabled(false);

  oauthClient = new GoogleDriveOauth;

  oauthClient->setup();

  // HACK:
  connect(oauthClient->google, &QOAuth2AuthorizationCodeFlow::granted,
          oauthClient, [this]() {
            // NOTE: this should change
            ui->cardButton->setEnabled(true);
            qDebug() << __FUNCTION__ << __LINE__ << "Access Granted!";
            qDebug() << oauthClient->google->token();
            qDebug() << oauthClient->google->expirationAt();
          });

  // TODO: this should only be invoked when needed
  oauthClient->google->refreshAccessToken();
}

void StaffWindow::addButton(QString title, QString name, int id, bool visible) {
  QPushButton *button = new QPushButton(title);
  button->setObjectName(name);
  button->setCheckable(true);
  if (!visible)
    button->hide();

  ui->verticalLayout->addWidget(button);
  buttonGroup->addButton(button, id);
}

void StaffWindow::removeButton(QAbstractButton *button) {
  buttonGroup->removeButton(button);
  ui->verticalLayout->removeWidget(button);
  delete button;
}

void StaffWindow::selectMember(const Staff &member) {

  for (const auto &[field, value] : std::unordered_map<QString, QString>{
           {"nationalIdField",
            member.getNationalId().has_value()
                ? QString::number(member.getNationalId().value())
                : QString()},
           {"idField", QString::number(member.getId())},
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

void StaffWindow::selectMember(unsigned int id) {
  selectMember(id == 0 ? Staff{}
                       : *find_if(staffMembers.begin(), staffMembers.end(),
                                  [id](const Staff &member) {
                                    return member.getId() == id;
                                  }));
}

void StaffWindow::clearForm() { selectMember(); }

void StaffWindow::loadMembersList(int chosenId) {

  staffMembers = fetchMembers(searchQuery, Staff::SearchCriteria::FullName,
                              sortDirection, Staff::OrderColumn::FirstName);

  buttonHistory = {};
  newMember = {};

  // Only display the form if there are any staff members or
  // if currently editing a 'new member'

  ui->formWidget->setVisible(!staffMembers.empty() ||
                             buttonGroup->button(0)->isVisible());

  // remove all buttons except for the 'new member' button
  foreach (const auto &button, buttonGroup->buttons().mid(1))
    removeButton(button);

  // add the new members
  for (const Staff &member : qAsConst(staffMembers))
    addButton(member.getFirstName() + ' ' + member.getLastName(),
              "staffMember" + QString::number(member.getId()), member.getId());

  // Clicking a desired member

  if (staffMembers.empty() || buttonGroup->buttons().length() <= 1)
    return;

  if (chosenId == -1) // default, meaning the first found member button
    buttonGroup->button(staffMembers.constFirst().getId())->click();
  else if (buttonGroup->button(chosenId)) // chosen button, must exist
    buttonGroup->button(chosenId)->click();
  else
    qWarning() << QString() + "Button " + chosenId + " not found";
}

Staff StaffWindow::memberFromForm() const {

  std::optional<Account> account = std::nullopt;

  if (!ui->usernameField->text().isEmpty()) {
    account.emplace();
    account.value().setUserName(ui->usernameField->text());
    account.value().setPassword(
        ui->passwordField->text().isEmpty()
            ? findMember(buttonGroup->checkedId()).account->getPassword()
            : Account::hashPassword(ui->passwordField->text()));
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


// TODO: implement
bool StaffWindow::validateSubmission() const { return true; }

bool StaffWindow::validateSubmission(const Staff &member, Submission kind) {
  // if( member.account->generatePassword ) {
  //
  // }
  return true;
}

Staff StaffWindow::findMember(unsigned int id) const {
  return *find_if(staffMembers.begin(), staffMembers.end(),
                  [id](const Staff &member) { return member.getId() == id; });
}

QChartView *StaffWindow::genChartView(Statistics category) const {

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

void StaffWindow::showStats() const {

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

void StaffWindow::showEmployeeCard() const {
  const Staff currMember = findMember(buttonGroup->checkedId());

  // create a QPainter to render the document

  QTextDocument document(this->parentWidget());
  document.setHtml(currMember.renderCardTemplate());

  QImage *htmlImage = new QImage(165, 200, QImage::Format_ARGB32_Premultiplied);
  htmlImage->fill(Qt::white);
  QPainter htmlPainter(htmlImage);
  document.drawContents(&htmlPainter);

  // create a QWidget to display the image

  QWidget *previewWidget = new QWidget(this->parentWidget());
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

  QPushButton *saveButton = new QPushButton(tr("Save"), previewWidget),
              *uploadButton = new QPushButton(tr("Upload"), previewWidget);

  connect(
      saveButton, &QPushButton::released, previewWidget,
      [label, currMember, previewWidget]() {
        const QString defaultName =
            tr("employeeCard") + QString::number(currMember.getId()) + ".png";

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
                                       QString::number(currMember.getId()) +
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

StaffWindow::~StaffWindow() { delete ui; }
