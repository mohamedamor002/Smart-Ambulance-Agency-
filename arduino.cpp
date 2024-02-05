#include "arduino.h"
#include <QSqlQuery>
#include <QCoreApplication>
#include <QThread>

using std::find_if, std::nullopt, std::optional;

Arduino::Arduino(const QSerialPortInfo &info) : m_serial(new QSerialPort(info)){};

// WARNING: this will not check if the port exists
Arduino::Arduino(const QString &serialNumber)
    : m_serial(new QSerialPort(
          Arduino::find(serialNumber).value_or(QSerialPortInfo()))){};

optional<QSerialPortInfo> Arduino::find(quint16 vendorId, quint16 productId) {
  const auto availablePorts = QSerialPortInfo::availablePorts();
  if (auto found = find_if(availablePorts.begin(), availablePorts.end(),
                           [vendorId, productId](const QSerialPortInfo &port) {
                             return port.hasProductIdentifier() &&
                                    port.hasVendorIdentifier() &&
                                    port.vendorIdentifier() == vendorId &&
                                    port.productIdentifier() == productId;
                           });
      found != availablePorts.end())
    return *found;

  return nullopt;
}

optional<QSerialPortInfo> Arduino::find(const QString &serialNumber) {
  const auto availablePorts = QSerialPortInfo::availablePorts();
  if (auto found = find_if(availablePorts.begin(), availablePorts.end(),
                           [&serialNumber](const QSerialPortInfo &port) {
                             return port.serialNumber() == serialNumber;
                           });
      found != availablePorts.end())
    return *found;

  return nullopt;
}

bool Arduino::connect() const {
  if (!m_serial->open(QSerialPort::ReadWrite))
    return false;
  m_serial->setBaudRate(QSerialPort::Baud9600); // bit rate: 9600 bits/s
  m_serial->setDataBits(QSerialPort::Data8);    // Data length: 8 bit,
  m_serial->setParity(QSerialPort::NoParity);   // 1 optional parity bit
  m_serial->setStopBits(QSerialPort::OneStop);  // Number of stop bit : 1
  m_serial->setFlowControl(QSerialPort::NoFlowControl);
  return true;
}

bool Arduino::close() const {

  if (!m_serial->isOpen())
    return false;

  m_serial->close();
  return true;
}

QByteArray Arduino::read() {
  if (!m_serial->isReadable())
    return {};
  data = m_serial->readAll();

  return data;
}

QByteArray Arduino::readline() {
  if (!m_serial->canReadLine())
    return {};
  data = m_serial->readLine();

  return data;
}

bool Arduino::write(const QByteArray &input) const {
  if (!m_serial->isWritable()) {
    qDebug() << "Couldn't write to serial!";
    return false;
  }
  m_serial->write(input);
  return true;
}

bool Arduino::updateScenario(int pos)
{
  QSqlQuery query ;

  query.prepare("UPDATE VALIDATION SET CURRENT_POSITION=:n WHERE ID=1")  ;
  query.bindValue(":n",pos );

  return query.exec() ;


}
int Arduino::getpos()
{
  int currentPos = -1; // set a default value in case the query fails

  QSqlQuery query;
  query.prepare("SELECT CURRENT_POSITION FROM VALIDATION WHERE ID=1");
  if (query.exec() && query.next()) { // execute the query and check if there is a result
    currentPos = query.value(0).toInt(); // retrieve the value of the first column in the result
  }

  return currentPos;
}

