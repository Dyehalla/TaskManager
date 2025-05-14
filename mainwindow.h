#include <QMainWindow>
#include <QString>
#include <QLabel>

// Форвард-декларация UI-класса (генерируется из .ui)
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    int process_widget_amount;
    int network_widget_amount;

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void resize_columns_to_content();
    void update_processes();

// private:
    Ui::MainWindow *ui;  // Указатель на сгенерированный UI
};
