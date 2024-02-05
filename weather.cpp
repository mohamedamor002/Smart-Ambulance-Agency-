#include "weather.h"

Weather::Weather(double feelsLike, double gnd, double temp, double tempMax,
                 double tempMin, double sea, double pressure, double hum,
                 QString location)
    : feelsLike(feelsLike), gnd(gnd), temperature(temp), tempMax(tempMax),
      tempMin(tempMin), sea(sea), pressure(pressure), humidity(hum),
      location(location) {}

void Weather::getInfos(const QUrlQuery &query,
                       std::function<void(const Weather &w)> callback) {
  QNetworkAccessManager *man = new QNetworkAccessManager();

  QUrl url("http://api.openweathermap.org/data/2.5/weather");

  url.setQuery(query);

  QNetworkRequest req(url);

  QNetworkReply *rep = man->get(req);

  QNetworkReply::connect(rep, &QNetworkReply::finished, rep, [=]() {
    const QByteArray all = rep->readAll();
    man->deleteLater();
    const QJsonObject obj = QJsonDocument::fromJson(all).object();
    const QJsonObject main = obj["main"].toObject();

    const double temp = main["temp"].toDouble(),
                 gnd = main["grnd_level"].toDouble(),
                 sea = main["sea_level"].toDouble(),
                 feelsLike = main["feels_like"].toDouble(),
                 pressure = main["pressure"].toDouble(),
                 tempMin = main["temp_min"].toDouble(),
                 hum = main["humidity"].toDouble(),
                 tempMax = main["temp_max"].toDouble();

    const QString location = obj["name"].toString();
    callback({
        feelsLike,
        gnd,
        temp,
        tempMax,
        tempMin,
        sea,
        pressure,
        hum,
        location,
    });
  });
}

void Weather::getInfos(const QGeoCoordinate &coord,
                       std::function<void(const Weather &w)> callback) {
  getInfos(
      {
          {"lon", QString::number(coord.longitude())},
          {"lat", QString::number(coord.latitude())},
          // {"q", option + ",tun"},
          {"units", "Metric"},
          {"appid", "66f1a2571adc7f4dcdda008c534bf9bb"},
      },
      callback);
}

void Weather::getInfos(const QString &location,
                       std::function<void(const Weather &w)> callback) {
  getInfos(
      {
          {"q", location},
          // {"q", option + ",tun"},
          {"units", "Metric"},
          {"appid", "66f1a2571adc7f4dcdda008c534bf9bb"},
      },
      callback);
}

// weather weather::getInfos(const QString &option) {
//     weather W;

//     CURL *curl;
//     CURLcode res;
//     curl = curl_easy_init();
//     std::string data;

//     // check if the cURL handle was created successfully
//     if(curl) {
//         // set the cURL options
//         curl_easy_setopt(curl, CURLOPT_URL,
//         ("http://api.openweathermap.org/data/2.5/weather?q="+option+",tun&units=metric&appid=66f1a2571adc7f4dcdda008c534bf9bb").toStdString().c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

//         // perform the request
//         res = curl_easy_perform(curl);

//         // check if the request was successful
//         if(res != CURLE_OK) {
//             qDebug() << "curl_easy_perform() failed: " <<
//             curl_easy_strerror(res);
//         } else {
//             // parse the JSON data using Qt's QJsonDocument class
//             QJsonDocument doc = QJsonDocument::fromJson(data.c_str());

//             // extract the weather data from the JSON object
//             QJsonObject mainObj = doc.object().value("main").toObject();
//             double temp = mainObj.value("temp").toDouble();
//             double gnd= mainObj.value("grnd_level").toDouble() ;
//             double sea= mainObj.value("sea_level").toDouble() ;

//             double feelsLike = mainObj.value("feels_like").toDouble();
//             double pressure = mainObj.value("pressure").toDouble();

//             double tempMin = mainObj.value("temp_min").toDouble();
//             double hum = mainObj.value("humidity").toDouble();

//             double tempMax = mainObj.value("temp_max").toDouble();
//             W = weather(feelsLike,gnd,temp,tempMax,tempMin,sea,pressure,hum)
//             ;

//             QJsonObject coordObj = doc.object().value("coord").toObject();
//             double lon = coordObj.value("lon").toDouble();
//             double lat = coordObj.value("lat").toDouble();
//             qDebug()<< mainObj ;
//             qDebug() << QString::number(lon) ;
//             qDebug() << QString::number(lat) ;
//             qDebug() << QString::number(temp) ;
//             qDebug() << QString::number(feelsLike) ;
//             qDebug() << QString::number(tempMin) ;

//             // display the weather data in the GUI
//             /* ui->lonLabel->setText(QString::number(lon));
//             ui->latLabel->setText(QString::number(lat));
//             ui->tempLabel->setText(QString::number(temp));
//             ui->feelsLikeLabel->setText(QString::number(feelsLike));
//             ui->tempMinLabel->setText(QString::number(tempMin));
//             ui->tempMaxLabel->setText(QString::number(tempMax));*/
//         }

//         // cleanup
//         curl_easy_cleanup(curl);
//     }

//     return W;
// }

// size_t Weather::writeCallback(char *ptr, size_t size, size_t nmemb,
//                               std::string *data) {
//   data->append(ptr, size * nmemb);
//   return size * nmemb;
// }
