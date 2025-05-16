#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QEventLoop>
#include <QCoreApplication>

void waitForReply(QNetworkReply* reply);

QString uploadFileToVirusTotal(QNetworkAccessManager &networkManager, const QString& filePath, const QString& apiKey);
QJsonObject getVirusTotalReport(QNetworkAccessManager &networkManager, const QString& analysisId, const QString& apiKey);

QJsonObject getVirusTotalReport(const QString& analysisId, const QString& apiKey);
