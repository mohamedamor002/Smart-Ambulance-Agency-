#include "staff.h"

// constructors

Staff::Staff(const std::optional<unsigned int> nationalId,
             const QString &firstName, const QString &lastName,
             const QString &gender, const QString &role, const QString &status,
             const QString &picture, const std::optional<Account> &account)
    : nationalId(nationalId), firstName(firstName), lastName(lastName),
      gender(gender), role(role), status(status), picture(picture),
      account(account) {}

Staff::Staff(const unsigned int id,
             const std::optional<unsigned int> nationalId,
             const QString &firstName, const QString &lastName,
             const QString &gender, const QString &role, const QString &status,
             const QString &picture, const std::optional<Account> &account)
    : Staff(nationalId, firstName, lastName, gender, role, status, picture,
            account) {
  this->id = id;
}

Staff::Staff(const QSqlQuery &query)
    : Staff(query.value("S_ID").toInt(),
            query.value("CIN").isNull()
                ? std::nullopt
                : std::optional<unsigned int>{query.value("CIN").toInt()},
            query.value("FNAME").toString(), query.value("LNAME").toString(),
            query.value("GENDER").toString(), query.value("ROLE").toString(),
            query.value("STATUS").toString(), query.value("PICTURE").toString(),
            query.value("HAS_ACCOUNT").toBool()
                ? std::optional<Account>{Account(
                      query.value("USERNAME").toString(),
                      query.value("PASSWORD").toString())}
                : std::nullopt) {}

Staff::Staff(const QSqlRecord &record)
    : Staff(record.value("S_ID").toInt(),
            record.value("CIN").isNull()
                ? std::nullopt
                : std::optional<unsigned int>{record.value("CIN").toInt()},
            record.value("FNAME").toString(), record.value("LNAME").toString(),
            record.value("GENDER").toString(), record.value("ROLE").toString(),
            record.value("STATUS").toString(),
            record.value("PICTURE").toString(),
            record.value("HAS_ACCOUNT").toBool()
                ? std::optional<Account>{Account(
                      record.value("USERNAME").toString(),
                      record.value("PASSWORD").toString())}
                : std::nullopt) {}

// CREATE

std::optional<unsigned int>
Staff::create(const std::optional<unsigned int> nationalId,
              const QString &firstName, const QString &lastName,
              const QString &gender, const QString &role, const QString &status,
              const QString &picture, const std::optional<Account> &account) {
  QSqlQuery query;
  query.prepare("BEGIN "
                "INSERT INTO STAFF "
                "(CIN, FNAME, LNAME, ROLE, GENDER, STATUS, PICTURE, "
                "HAS_ACCOUNT, USERNAME, PASSWORD) "
                "VALUES(:CIN, :FNAME,:LNAME,:ROLE,:GENDER,:STATUS,:PICTURE, "
                ":HAS_ACCOUNT, :USERNAME, :PASSWORD) "
                "RETURNING S_ID INTO :S_ID;"
                "END;");

  query.bindValue(":CIN", nationalId.has_value() ? nationalId.value()
                                                 : QVariant(QVariant::Int));
  query.bindValue(":FNAME", firstName);
  query.bindValue(":LNAME", lastName);
  query.bindValue(":GENDER", gender);
  query.bindValue(":ROLE", role);
  query.bindValue(":STATUS", status);
  query.bindValue(":PICTURE", picture);
  query.bindValue(":HAS_ACCOUNT", account.has_value());
  query.bindValue(":USERNAME", account.has_value()
                                   ? account.value().getUserName()
                                   : QVariant(QVariant::String));
  query.bindValue(":PASSWORD", account.has_value()
                                   ? account.value().getPassword()
                                   : QVariant(QVariant::String));
  query.bindValue(":S_ID", 0, QSql::Out);

  if (!query.exec()) {
    ; // TODO: handle error
    return std::nullopt;
  }
  return query.boundValue(":S_ID").toInt();
}

bool Staff::update(const unsigned int id,
                   const std::optional<unsigned int> nationalId,
                   const QString &firstName, const QString &lastName,
                   const QString &gender, const QString &role,
                   const QString &status, const QString &picture,
                   const std::optional<Account> &account) {
  QSqlQuery query;
  query.prepare("UPDATE STAFF "
                "SET CIN=:CIN, "
                "FNAME=:FNAME, "
                "LNAME=:LNAME, "
                "GENDER=:GENDER, "
                "ROLE=:ROLE, "
                "STATUS=:STATUS, "
                "USERNAME=:USERNAME, "
                "PASSWORD=:PASSWORD, "
                "HAS_ACCOUNT=:HAS_ACCOUNT "
                "WHERE S_ID=:S_ID");

  query.bindValue(":S_ID", id);
  query.bindValue(":CIN", nationalId.has_value() ? nationalId.value()
                                                 : QVariant(QVariant::Int));
  query.bindValue(":FNAME", firstName);
  query.bindValue(":LNAME", lastName);
  query.bindValue(":ROLE", role);
  query.bindValue(":GENDER", gender);
  query.bindValue(":STATUS", status);
  query.bindValue(":PICTURE", picture);
  query.bindValue(":HAS_ACCOUNT", account.has_value());
  query.bindValue(":USERNAME", account.has_value()
                                   ? account.value().getUserName()
                                   : QVariant(QVariant::String));
  query.bindValue(":PASSWORD", account.has_value()
                                   ? account.value().getPassword()
                                   : QVariant(QVariant::String));
  if (!query.exec()) {
    ; // TODO: handle error
    return false;
  }
  return true;
}

bool Staff::exists(const unsigned int id) {
  QSqlQuery query;
  query.prepare("SELECT S_ID FROM staff WHERE S_ID=?");
  query.addBindValue(id);
  if (!query.exec()) {
    ; // check for error
  }
  return query.first();
}

// ------FIND------

std::optional<Staff> Staff::find(const unsigned int id) {
  QSqlQuery query;
  query.prepare("SELECT * FROM staff WHERE S_ID=?");
  query.addBindValue(id);
  if (!query.exec()) {
    ; // check for error
  }

  if (!query.first())
    return std::nullopt;

  return Staff(query);
}

std::optional<Staff> Staff::find(const QString &username) {
  QSqlQuery query;
  query.prepare("SELECT * FROM staff WHERE HAS_ACCOUNT=1 AND "
                "USERNAME=:USERNAME");
  query.bindValue(":USERNAME", username);
  if (!query.exec()) {
    // check for error
  }

  if (!query.first())
    return std::nullopt;

  return Staff(query);
}

std::optional<Staff> Staff::find(const QString &username,
                                 const QString &hashedPassword) {

  QSqlQuery query;
  query.prepare("SELECT * FROM staff WHERE HAS_ACCOUNT=1 AND "
                "USERNAME=:USERNAME AND PASSWORD=:PASSWORD");
  query.bindValue(":USERNAME", username);
  query.bindValue(":PASSWORD", hashedPassword);
  if (!query.exec()) {
    // check for error
  }

  if (!query.first())
    return std::nullopt;

  return Staff(query);
}

QSqlQuery Staff::findAll(const QString &search, SearchCriteria criteria,
                         Qt::SortOrder orderDirection,
                         Staff::OrderColumn orderColumn,
                         CategorizeBy columnCategory, const QString &category) {

  QSqlQuery query;

  // TODO: place this in the class somewhere
  const QMap<Staff::OrderColumn, QString> columnNames{
      {Staff::OrderColumn::LastName, "LNAME"},
      {Staff::OrderColumn::FirstName, "FNAME"},
  };

  // the search is case insensitive
  const QString queryString =
      QString("SELECT FNAME || ' ' || LNAME FULLNAME, s.* FROM STAFF s%1 ORDER "
              "BY %2 %3")
          .arg((search.isNull()
                    ? QString()
                    : (" WHERE " + ([criteria]() -> QString {
                         switch (criteria) {
                         case SearchCriteria::FirstName:
                           return "UPPER(FNAME) LIKE "
                                  "'%'||UPPER(:search)||'%'";
                         case SearchCriteria::LastName:
                           return "UPPER(LNAME) LIKE "
                                  "'%'||UPPER(:search)||'%'";
                         case SearchCriteria::FullName:
                           return "UPPER(FNAME || ' ' || LNAME) LIKE "
                                  "'%'||UPPER(:search)||'%'"
                                  " OR "
                                  "UPPER(LNAME || ' ' || FNAME) LIKE "
                                  "'%'||UPPER(:search)||'%'";
                         default: {
                           return "FNAME LIKE '%'||?||'%'";
                         }
                         }
                       })())),
               columnNames.value(orderColumn),
               ((orderDirection == Qt::AscendingOrder) ? "ASC" : "DESC"));

  query.prepare(queryString);

  if (!search.isNull())
    query.bindValue(":search", search);

  if (!query.exec()) {
    ;
  }

  return query;
}

bool Staff::remove(const unsigned int id) {
  QSqlQuery query;
  query.prepare("DELETE FROM staff WHERE S_ID=?");
  query.addBindValue(id);
  return query.exec();
}

// member methods

std::optional<unsigned int> Staff::create() const {
  return Staff::create(nationalId, firstName, lastName, gender, role, status,
                       picture, account);
}

bool Staff::update() const {
  return Staff::update(id, nationalId, firstName, lastName, gender, role,
                       status, picture, account);
}

bool Staff::exists() const { return Staff::exists(id); }

std::optional<Staff> Staff::find(FindBy criteria) const {
  return criteria == FindBy::Id
             ? Staff::find(id)
             : Staff::find(account->getUserName(), account->getPassword());
}

bool Staff::remove() const { return Staff::remove(id); }

unsigned int Staff::totals(const QString &column, const QVariant &value) {
  QSqlQuery query;

  const QString queryString =
      QString("SELECT COUNT(*) FROM STAFF WHERE %1 = :VALUE").arg(column);
  query.prepare(queryString);
  query.bindValue(":VALUE", value);

  query.exec();

  query.first();

  return query.value(0).toUInt();
}

QString Staff::renderCardTemplate() const {
  QFile templateFile(QFile(":/rsc/templates/employeecard-template.html"));
  templateFile.open(QIODevice::ReadOnly);

  const QString rawTemplate = templateFile.readAll();

  templateFile.close();

  const QString placeholder = ":/rsc/templates/avatar_small.png";
  return rawTemplate.arg(firstName + ' ' + lastName, role,
                         picture.isEmpty() ? placeholder : picture);
}
