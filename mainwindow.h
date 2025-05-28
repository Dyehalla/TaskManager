#include <QMainWindow>
#include <QString>
#include <QLabel>
#include <QNetworkAccessManager>
#include <iostream>
#include <QDebug>
#include <QThread>
#include <QStandardItemModel>
#include <vector>
#include <unordered_map>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QSortFilterProxyModel>
#include <QTableView>

#define PROCESS_TABLE 0
#define NETWORK_TABLE 1

int extractLeadingDigits(const QString& str);

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
    QTableView *table;

    NonEditableModel *model;
    QNetworkAccessManager networkManager;
    QTimer *timer;

    int current_table = -1;
    int process_sort = 2;
    int network_sort = 2;
    int refresh_type = 4;
    std::vector<std::wstring> path_vector;
    std::unordered_map<int, std::vector<unsigned long>> pid_map;
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
    void timeout();
    void change_timer();
    void header_click(int column);
    void change_table();
};
