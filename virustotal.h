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
#include <QSettings>
#include <QInputDialog>
#include <QMessageBox>

void waitForReply(QNetworkReply* reply);

QString uploadFileToVirusTotal(QNetworkAccessManager &networkManager, QString& filePath, QString& apiKey);
QString uploadBigFileToVirusTotal(QNetworkAccessManager &networkManager, QString& filePath, QString& apiKey);
QJsonObject getVirusTotalReport(QNetworkAccessManager &networkManager, QString& analysisId, QString& apiKey);
