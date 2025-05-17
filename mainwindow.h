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

#define PROCESS_TABLE 0
#define NETWORK_TABLE 1

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
    NonEditableModel *model;
    QNetworkAccessManager networkManager;

    int current_table = PROCESS_TABLE;
    std::vector<std::wstring> path_vector;

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

// private:
    Ui::MainWindow *ui;  // Указатель на сгенерированный UI

};
