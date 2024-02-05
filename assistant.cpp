#include <QIcon>
#include <QModelIndex>
#include <QScrollBar>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QVariant>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "assistant.h"
#include "curl/curl.h"

assistant::assistant()
{

}
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    QString *response = reinterpret_cast<QString *>(userdata);
    response->append(QString::fromUtf8(ptr, size * nmemb));
    return size * nmemb;
}


void assistant::generate_and_display_text(const QString &prompt,
                                           const QString &model,
                                           const QString &api_key,
                                           QPlainTextEdit *label) {
    QString response;
    QString url =
        QString("https://api.openai.com/v1/engines/%1/completions").arg(model);
    QString data =
        QString("{\"prompt\": \"%1\",\"temperature\": 0.5,\"max_tokens\": 50}")
            .arg(prompt);

    struct curl_slist *header_list = nullptr;
    header_list = curl_slist_append(
        header_list, "Content-Type: application/json"); // Set Content-Type header
    header_list = curl_slist_append(
        header_list,
        QString("Authorization: Bearer %1").arg(api_key).toStdString().c_str());

    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.toStdString().c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.toStdString().c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            qDebug() << "Failed to send request: " << curl_easy_strerror(result);
        }
        curl_easy_cleanup(curl);
    }
    curl_slist_free_all(header_list);

    QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray choicesArray = jsonObj["choices"].toArray();
    QString text = choicesArray.at(0).toObject()["text"].toString();
    label->setPlainText(text);
}
