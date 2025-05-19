#include "timer_dialog.h"
#include "ui_timer_dialog.h"

TimerDialog::TimerDialog(QWidget *parent, int* user_value) : QDialog(parent), ui(new Ui::TimerDialog) {
    ui->setupUi(this);
    connect(ui->confirmButton, &QPushButton::clicked, this, &TimerDialog::confirm);
    ui->slider->setValue(*user_value);
    return_value = user_value;
}

void TimerDialog::confirm() {
    int val = ui->slider->value();
    *return_value = val;
    reject();
}
