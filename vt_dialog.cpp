#include "vt_dialog.h"
#include "ui_vt_dialog.h"
#include "virustotal.h"


void VtDialog::stop_button(){
    cancelled = true;
    reject();
}

VtDialog::VtDialog(QWidget* parent, std::wstring& process_path) : QDialog(parent), ui(new Ui::VtDialog) {
    ui->setupUi(this);
    connect(ui->stopButton, &QPushButton::clicked, this, &VtDialog::stop_button);
    path = QString::fromStdWString(process_path);

    setWindowTitle("Анализ файла");
    ui->textEdit->setReadOnly(true);
}

VtDialog::~VtDialog() {
    delete ui;
}

void VtDialog::start_analysis() {
    long long size = QFileInfo(path).size();
    if (size > 650000000){
        QMessageBox::critical(this, "Ошибка", "Файл превышает размер 650Мб");
    }
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
    QString apiKey = get_virustotal_api_key();
    if (apiKey == "") {
        reject();
        return;
    }

    // Показываем статус
    ui->status_label->setText("Загрузка файла...");

    QString text;
    text += QString("Файл: %1\n").arg(QFileInfo(path).fileName());
    text += QString("Размер: %1 байт\n\n").arg(size);
    ui->textEdit->setPlainText(text);

    // Загружаем файл
    QString analysisId;
    if (size < 32000000){
        analysisId = uploadFileToVirusTotal(*networkManager, path, apiKey);
    }
    else {
        analysisId = uploadBigFileToVirusTotal(*networkManager, path, apiKey);
    }

    if (cancelled) return;

    if (analysisId.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить файл на VirusTotal");
        reject();
        return;
    }

    ui->status_label->setText("Ожидание результатов...");

    QJsonObject report;
    // int attempts = 0;
    // const int maxAttempts = 20;
    const int delayMs = 2000; // проверка каждые 2 секунды

    while (true) {
        QEventLoop loop;
        QTimer::singleShot(delayMs, &loop, &QEventLoop::quit);
        loop.exec();
        if (cancelled) return;
        report = getVirusTotalReport(*networkManager, analysisId, apiKey);
        if (cancelled) return;
        QString status = report["data"].toObject()["attributes"].toObject()["status"].toString();
        qDebug() << status;
        if (status == "completed") {
            break;
        }
        // attempts++;
    }

    if (report.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить результаты анализа файла");
        reject();
        return;
    }

    // Отображаем результаты
    display_results(report);
}

void VtDialog::display_results(const QJsonObject& report) {
    ui->status_label->setText("Анализ завершён");

    const QJsonObject attributes = report["data"].toObject()["attributes"].toObject();
    const QJsonObject stats = attributes["stats"].toObject();
    const QJsonObject results = attributes["results"].toObject();

    QString resultText;
    resultText += QString("Результаты анализа\n\n");
    resultText += QString("Файл: %1\n").arg(QFileInfo(path).fileName());
    resultText += QString("Размер: %1 байт\n\n").arg(QFileInfo(path).size());

    resultText += QString("Detection stats:\n");
    resultText += QString("  Вредоносный: %1\n").arg(stats["malicious"].toInt());
    resultText += QString("  Подозрительный: %1\n").arg(stats["suspicious"].toInt());
    resultText += QString("  Угроз не обнаружено: %1\n").arg(stats["undetected"].toInt());
    resultText += QString("  Безвредный: %1\n\n").arg(stats["harmless"].toInt());

    if (stats["malicious"].toInt() > 0) {
        resultText += "Detected by:\n";

        for (const QString& engine : results.keys()) {
            const QJsonObject engineResult = results[engine].toObject();
            if (engineResult["category"].toString() == "malicious" ||
                engineResult["category"].toString() == "suspicious") {
                resultText += QString("  %1: %2 (%3)\n")
                .arg(engine)
                    .arg(engineResult["result"].toString())
                    .arg(engineResult["category"].toString());
            }
        }
    } else {
        resultText += "Угрозы не найдены.\n";
    }

    ui->textEdit->setPlainText(resultText);
    ui->stopButton->setText("Закрыть");
}

QString VtDialog::get_virustotal_api_key(){
    QSettings settings;
    QString apiKey = settings.value("VirusTotal/apiKey").toString();

    if (!apiKey.isEmpty()) {
        return apiKey;
    }

    return set_virustotal_api_key();
}

QString VtDialog::set_virustotal_api_key(){
    QSettings settings;
    bool ok;
    QString apiKey = QInputDialog::getText(this,
                                   "VirusTotal API Key",
                                   "Введите ваш API ключ для VirusTotal:",
                                   QLineEdit::Normal,
                                   "",
                                   &ok);

    if (!ok || apiKey.isEmpty()) {
        QMessageBox::warning(this,
                             "Ошибка",
                             "API ключ не был введен. Функционал VirusTotal недоступен.");
        return "";
    }

    // Сохраняем ключ
    settings.setValue("VirusTotal/apiKey", apiKey);

    return apiKey;
}


