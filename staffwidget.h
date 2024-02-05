#ifndef STAFFWIDGET_H
#define STAFFWIDGET_H

#include "googledriveoauth.h"
#include "staff.h"
#include "ui_staffwidget.h"
#include <QMainWindow>
#include <QSqlQueryModel>
#include <QWidget>
#include <QtCharts>

namespace Ui {
class StaffWidget;
}

class StaffWidget : public QWidget {
  Q_OBJECT

public:
  explicit StaffWidget(QWidget *parent = nullptr);

  // Listing

  void loadMembersList(int chosenId = -1);
  void loadMembersList(const QModelIndex &index);

  ~StaffWidget();

private:
  Ui::StaffWidget *ui;

  QSqlQueryModel *model = new QSqlQueryModel(this);

  // Form

  enum class FormMode { Editing, NewMember } mode = FormMode::NewMember;

  void clearForm();
  Staff memberFromForm() const;
  void loadMemberOnForm(const Staff &member);
  std::optional<Staff> currentySelectedListMember() const;
  // void editNewMember(FormMode mode);
  // void editExistingMember(FormMode mode);

  // searching

  QString searchQuery() const { return ui->searchBox->text(); }

  // sorting

  Qt::SortOrder sortDirection = Qt::AscendingOrder;
  enum class Submission { Update, Creation };
  enum class Statistics { Gender, Role, Account };

  // Stats

  QChartView *genChartView(Statistics category) const;
  void showStats() const;

  // Employee card

  void showEmployeeCard() const;

  GoogleDriveOauth *oauthClient;
};

#endif // STAFFWIDGET_H
