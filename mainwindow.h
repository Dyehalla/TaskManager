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
    std::vector<QLabel*> exe_name_label_buffer;
    std::vector<QLabel*> exe_path_label_buffer;
    std::vector<QLabel*> memory_label_buffer;
    std::vector<QLabel*> cpu_label_buffer;

    int process_widget_amount;

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void create_process_widget();
    void delete_process_widget();
    void update_processes();

private:
    Ui::MainWindow *ui;  // Указатель на сгенерированный UI
};
