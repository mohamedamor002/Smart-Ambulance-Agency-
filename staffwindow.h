#ifndef STAFFWINDOW_H
#define STAFFWINDOW_H

#include "config.h"
#include "logindialog.h"
#include "googledriveoauth.h"
#include "staff.h"
#include <QAbstractButton>
#include <QAction>
#include <QButtonGroup>
#include <QDate>
#include <QDebug>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QPair>
#include <QPieSlice>
#include <QPushButton>
#include <QRandomGenerator>
#include <QStack>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextFrame>
#include <QTextTable>
#include <QVBoxLayout>
#include <QtCharts>
#include <QtGlobal>
#include <QtNetwork>
#include <algorithm>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class StaffWindow : public QMainWindow {
  Q_OBJECT

public:
  StaffWindow(QWidget *parent = nullptr, const Staff &member = {});
  ~StaffWindow();

  Ui::MainWindow *ui;

private:
  void addButton(QString title, QString name, int id = -1, bool visible = true);
  void removeButton(QAbstractButton *button);
  void selectMember(const Staff &member = {});
  void selectMember(unsigned int id);
  static inline auto fetchMembers = Staff::findAll;
  // load or reload
  void loadMembersList(int chosenId = -1);
  void clearForm();
  Staff memberFromForm() const;
  QButtonGroup *buttonGroup = new QButtonGroup(this);
  Staff newMember = {};
  QVector<Staff> staffMembers;
  // QVector<QPair<unsigned int, Staff>> staffMembers;
  QStack<int> buttonHistory;
  Qt::SortOrder sortDirection = Qt::AscendingOrder;
  QString searchQuery;
  bool validateSubmission() const;
  enum class Submission { Update, Creation };

  // bool verifyLogin() const;
  static bool validateSubmission(const Staff &member, Submission kind);
  bool validateSubmission();
  Staff findMember(unsigned int id) const;
  enum class Statistics { Gender, Role, Account };
  QChartView *genChartView(Statistics category) const;
  void showStats() const;
  void showEmployeeCard() const;

  GoogleDriveOauth *oauthClient;
};
#endif // STAFFWINDOW_H
