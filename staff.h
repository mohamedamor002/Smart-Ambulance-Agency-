#ifndef STAFF_H
#define STAFF_H

#include "account.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QString>
#include <QtGlobal>
#include <optional>

// Database definition
// S_ID
// CIN
// FNAME
// LNAME
// PICTURE
// GENDER
// STATUS
// ROLE
// HAS_ACCOUNT
// USERNAME
// PASSWORD

class Staff {
  unsigned int id = 0;
  std::optional<unsigned int> nationalId = std::nullopt;
  QString firstName, lastName;
  QString gender;
  QString role;
  QString status; // TODO: decide on placeholder
  QString picture;

public:
  std::optional<Account> account = std::nullopt;
  Staff() = default;

  Staff(const std::optional<unsigned int> nationalId, const QString &firstName,
        const QString &lastName, const QString &gender,
        const QString &role = QString(), const QString &status = QString(),
        const QString &picture = QString(),
        const std::optional<Account> &account = std::nullopt);

  Staff(const unsigned int id, const std::optional<unsigned int> national_id,
        const QString &first_name, const QString &last_name,
        const QString &gender, const QString &role = QString(),
        const QString &status = QString(), const QString &picture = QString(),
        const std::optional<Account> &account = std::nullopt);

  Staff(const QSqlQuery &query);
  Staff(const QSqlRecord &record);

  Staff(Staff &&) = default;
  Staff(const Staff &) = default;
  Staff &operator=(Staff &&) = default;
  Staff &operator=(const Staff &) = default;
  ~Staff() = default;

  // enum

  enum class SearchCriteria { FirstName, LastName, FullName };
  enum class OrderColumn { FirstName, LastName };
  enum class FindBy { Id, Credentials };
  enum class CategorizeBy { Role, Status, Gender, Any };

  // database

  // static methods

  static std::optional<unsigned int>
  create(const std::optional<unsigned int> nationalId, const QString &firstName,
         const QString &lastName, const QString &gender,
         const QString &role = QString(), const QString &status = QString(),
         const QString &picture = QString(),
         const std::optional<Account> &account = std::nullopt);

  static bool
  update(const unsigned int id, const std::optional<unsigned int> nationalId,
         const QString &firstName, const QString &lastName,
         const QString &gender, const QString &role = QString(),
         const QString &status = QString(), const QString &picture = QString(),
         const std::optional<Account> &account =
             std::nullopt); // WARNING: omitted values will be set to null

  // static Staff find(unsigned int id);
  static std::optional<Staff> find(const unsigned int id);
  static std::optional<Staff> find(const QString &username);
  static std::optional<Staff> find(const QString &username,
                                   const QString &hashedPassword);
  static QSqlQuery
  findAll(const QString &search = QString(),
          SearchCriteria criteria = SearchCriteria::FullName,
          Qt::SortOrder orderDirection = Qt::AscendingOrder,
          Staff::OrderColumn orderColumn = Staff::OrderColumn::FirstName,
          CategorizeBy columnCategory = CategorizeBy::Any, const QString &category = QString());
  static bool remove(const unsigned int id);
  static bool exists(const unsigned int id);

  static unsigned int totals(const QString &column, const QVariant &value);

  // card
  // static QString renderCardTemplate(const Staff &member);

  // member methods

  std::optional<unsigned int> create() const;
  std::optional<Staff> find(FindBy = FindBy::Id) const;
  bool exists() const;
  bool update() const;
  bool remove() const;

  // accessors and mutators

  unsigned int getId() const { return id; }
  std::optional<unsigned int> getNationalId() const { return nationalId; }
  QString getFirstName() const { return firstName; }
  QString getLastName() const { return lastName; }
  // QString getFullName() const { return firstName + ' ' + lastName; }
  QString getGender() const { return gender; }
  QString getRole() const { return role; }
  QString getStatus() const { return status; }
  QString getPicture() const { return picture; }
  std::optional<Account> const constAccount() const { return account; }

  void setId(const unsigned int id) { this->id = id; }
  void setNationalId(const unsigned int nationalId) {
    this->nationalId = nationalId;
  }
  void setFirstName(const QString &firstName) { this->firstName = firstName; }
  void setLastName(const QString &lastName) { this->lastName = lastName; }
  void setGender(const QString &gender) { this->gender = gender; }
  void setRole(const QString &role) { this->role = role; }
  void setStatus(const QString &status) { this->status = status; }
  void setPicture(const QString &picture) { this->picture = picture; }

  // utility
  bool hasAccount() const { return account.has_value(); }

  // card
  QString renderCardTemplate() const;
};

#endif // STAFF
