#include <QMainWindow>
#include <QString>

// Форвард-декларация UI-класса (генерируется из .ui)
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void create_process_widget(const char * ExeName, const char * ExePath, const char * Memory, const char * CPU);


private:
    Ui::MainWindow *ui;  // Указатель на сгенерированный UI
};
