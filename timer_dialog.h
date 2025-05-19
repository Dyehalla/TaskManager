#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Ui {
class TimerDialog;
}

class TimerDialog : public QDialog {
    Q_OBJECT

public:
    TimerDialog(QWidget *parent = nullptr, int* user_value = nullptr);
    void confirm();
    Ui::TimerDialog *ui;
    int* return_value;
};



