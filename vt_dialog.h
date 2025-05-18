#include <QDialog>
#include <QString>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTimer>
#include <QSettings>

namespace Ui {
class VtDialog;
}

class VtDialog : public QDialog
{
    Q_OBJECT

public:
    VtDialog(QWidget* parent, std::wstring& process_path);
    ~VtDialog();

    Ui::VtDialog *ui;
    QString path;
    bool cancelled = false;

    void start_analysis();
    void stop_button();
    void display_results(const QJsonObject& report);

    QString get_virustotal_api_key();
    QString set_virustotal_api_key();
};
