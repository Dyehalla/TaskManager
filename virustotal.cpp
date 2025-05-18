#include "virustotal.h"

// Функция для ожидания завершения запроса
void waitForReply(QNetworkReply* reply) {
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
}

// Функция загрузки файла на VirusTotal
QString uploadFileToVirusTotal(QNetworkAccessManager &networkManager, QString& filePath, QString& apiKey) {
    apiKey.remove(QChar('\"'));
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл:" << filePath;
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
        qDebug() << doc;
        analysisId = doc.object()["data"].toObject()["id"].toString();
    } else {
        QMessageBox::critical(nullptr, "Ошибка", "Ваш API ключ не действителен");
        return "";
        qWarning() << "Ошибка загрузки:" << reply->errorString();
    }

    reply->deleteLater();
    return analysisId;
}

// Функция загрузки большого (>32mb) файла на VirusTotal
QString uploadBigFileToVirusTotal(QNetworkAccessManager &networkManager, QString& filePath, QString& apiKey) {
    apiKey.remove(QChar('\"'));
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл:" << filePath;
        delete file;
        return "";
    }

    // Получаем ссылку для загрузки большого файла
    QNetworkRequest request(QUrl("https://www.virustotal.com/api/v3/files/upload_url"));
    request.setRawHeader("x-apikey", apiKey.toUtf8());

    QNetworkReply* reply = networkManager.get(request);
    waitForReply(reply);

    QJsonObject report;
    if (reply->error() == QNetworkReply::NoError) {
        report = QJsonDocument::fromJson(reply->readAll()).object();
    } else{
        QMessageBox::critical(nullptr, "Ошибка", "Ваш API ключ не действителен");
        return "";
    }
    QString url_for_file = report["data"].toString();

    // Шлём файл
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"file\"; filename=\"" + QFileInfo(filePath).fileName() + "\""));
    filePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(filePart);

    QNetworkRequest post_request = QNetworkRequest(QUrl(url_for_file));
    post_request.setRawHeader("accept", "application/json");
    post_request.setRawHeader("x-apikey", apiKey.toUtf8());

    QNetworkReply* post_reply = networkManager.post(post_request, multiPart);
    multiPart->setParent(post_reply);

    waitForReply(post_reply);

    QString analysisId;
    if (post_reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(post_reply->readAll());

        analysisId = doc.object()["data"].toObject()["id"].toString();
    } else {
        qWarning() << "Ошибка загрузки:" << post_reply->errorString();
    }
    post_reply->deleteLater();
    return analysisId;
}

// Функция получения отчета анализа
QJsonObject getVirusTotalReport(QNetworkAccessManager &networkManager, QString& analysisId, QString& apiKey) {
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


