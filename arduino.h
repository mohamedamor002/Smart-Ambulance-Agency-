#ifndef ARDUINO_H
#define ARDUINO_H
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <algorithm>
#include <optional>
#include <QSqlQuery>

// NOTE: Serial numbers

// RFID - Buzzer: 24238313036351212111
// Traffic Lights: 7533531343735181C150
// Heart Monitor: 95032303737351E08242

class Arduino {
public:

  // Serial Numbers
  static constexpr const char * Traffic = "7533531343735181C150";
  static constexpr const char * Login = "24238313036351212111";
  static constexpr const char * HeartMonitor = "95032303737351E08242";


  Arduino() = default;
  Arduino(const QSerialPortInfo &info);
  Arduino(const QString &serialNumber);
  Arduino(Arduino &&) = default;
  Arduino(const Arduino &) = default;
  Arduino &operator=(Arduino &&) = default;
  Arduino &operator=(const Arduino &) = default;
  bool read_heart_pulse_and_insert_into_db();

  ~Arduino() = default;

  QSerialPort *serial() const { return m_serial; };
  bool connect() const;
  bool close() const;
  bool write(const QByteArray &input) const;
  QByteArray read();
  QByteArray readline();
  bool updateScenario(int pos) ;
  int getpos()  ;

  static std::optional<QSerialPortInfo> find(quint16 vendorId,
                                             quint16 productId);
  static std::optional<QSerialPortInfo> find(const QString &serialNumber);

  // static inline auto findAll = QSerialPortInfo::availablePorts;

private:
  QSerialPort *m_serial = new QSerialPort;
  quint16 vendorId = 9025;
  quint16 productId = 67;
  // bool available = false;
  QByteArray data; // TODO: remove?
};

#endif // ARDUINO_H
