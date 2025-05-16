#include "virustotal.h"

// Функция для ожидания завершения запроса
void waitForReply(QNetworkReply* reply) {
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
}

// Функция загрузки файла на VirusTotal
QString uploadFileToVirusTotal(QNetworkAccessManager &networkManager, const QString& filePath, const QString& apiKey) {
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        delete file;
        return "";
    }

    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(filePath).fileName() + "\""));
    filePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(filePart);

    QNetworkRequest request(QUrl("https://www.virustotal.com/api/v3/files"));
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("x-apikey", apiKey.toUtf8());

    QNetworkReply* reply = networkManager.post(request, multiPart);
    multiPart->setParent(reply);

    waitForReply(reply);

    QString analysisId;
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        analysisId = doc.object()["data"].toObject()["id"].toString();
    } else {
        qWarning() << "Upload error:" << reply->errorString();
    }

    reply->deleteLater();
    return analysisId;
}

// Функция получения отчета анализа
QJsonObject getVirusTotalReport(QNetworkAccessManager &networkManager, const QString& analysisId, const QString& apiKey) {
    QNetworkRequest request(QUrl("https://www.virustotal.com/api/v3/analyses/" + analysisId));
    request.setRawHeader("accept", "application/json");
    request.setRawHeader("x-apikey", apiKey.toUtf8());

    QNetworkReply* reply = networkManager.get(request);
    waitForReply(reply);

    QJsonObject report;
    if (reply->error() == QNetworkReply::NoError) {
        report = QJsonDocument::fromJson(reply->readAll()).object();
    } else {
        qWarning() << "Report error:" << reply->errorString();
    }

    reply->deleteLater();
    return report;
}

