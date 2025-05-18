#include <QMainWindow>
#include <QString>
#include <QLabel>
#include <QNetworkAccessManager>
#include <iostream>
#include <QDebug>
#include <QThread>
#include <QStandardItemModel>
#include <vector>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QSortFilterProxyModel>

#define PROCESS_TABLE 0
#define NETWORK_TABLE 1

int extractLeadingDigits(const QString& str);

class CustomSortModel : public QSortFilterProxyModel {

public:
    int* current_table;
    CustomSortModel(QObject* parent = nullptr, int* table = nullptr){
        current_table = table;
    }

protected:
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override {
        QString ldata = left.data().toString();
        QString rdata = right.data().toString();
        int col = left.column();
        if (col == 1 || (col == 2 && *current_table == NETWORK_TABLE)){
            int ldata_int = extractLeadingDigits(ldata);
            int rdata_int = extractLeadingDigits(rdata);
            if (ldata.contains("Мб")) ldata_int *= 1000;
            if (rdata.contains("Мб")) rdata_int *= 1000;
            return ldata_int < rdata_int;
        }
        if ((col == 3 && *current_table == PROCESS_TABLE) || col == 5){
            int ldata_int = extractLeadingDigits(ldata);
            int rdata_int = extractLeadingDigits(rdata);
            return ldata_int < rdata_int;
        }
        return QSortFilterProxyModel::lessThan(left, right);
    }
};

class NonEditableModel : public QStandardItemModel {
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QStandardItemModel::flags(index) & ~Qt::ItemIsEditable;
    }
};

// Форвард-декларация UI-класса (генерируется из .ui)
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    Ui::MainWindow *ui;  // Указатель на сгенерированный UI

    NonEditableModel *model;
    CustomSortModel* sort_model;
    QNetworkAccessManager networkManager;

    int current_table = -1;
    std::vector<std::wstring> path_vector;
    std::vector<int> process_col_width = {};
    std::vector<int> network_col_width = {};

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init_table();
    void resize_columns_to_content();
    void update_processes();
    void draw_network_table();
    void draw_process_table();
    void update_networks();
    void update_button();
    void erase_column(int col);
    void test();
    void update_table_rows_amount(int new_rows_count);
    void showContextMenu(const QPoint& pos);
    void vt_check(std::wstring path);
    void set_virustotal_api_key();

    void header_click(int column);

};
