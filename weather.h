#ifndef WEATHER_H
#define WEATHER_H

#include <QDebug>
#include <QGeoCoordinate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork>

struct Weather {
  Weather() = default;
  double feelsLike, gnd, temperature, tempMax, tempMin, sea, pressure, humidity;
  Weather(double feelsLike, double gnd, double temp, double tempMax,
          double tempMin, double sea, double pressure, double hum,
          QString location);
  QString location;
  static void getInfos(const QUrlQuery &query,
                       std::function<void(const Weather &w)> callback);
  static void getInfos(const QString &location,
                       std::function<void(const Weather &w)> callback);
  static void getInfos(const QGeoCoordinate &coord,
                       std::function<void(const Weather &w)> callback);

private:
  // static size_t writeCallback(char *ptr, size_t size, size_t nmemb,
  //                             std::string *data);
};

#endif // WEATHER_H
