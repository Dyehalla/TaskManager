#include <QMainWindow>
#include <QString>
#include <QLabel>
#include <QNetworkAccessManager>
#define PROCESS_TABLE 0
#define NETWORK_TABLE 1

// Форвард-декларация UI-класса (генерируется из .ui)
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    QNetworkAccessManager networkManager;
    int current_table = PROCESS_TABLE;

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

// private:
    Ui::MainWindow *ui;  // Указатель на сгенерированный UI

};
