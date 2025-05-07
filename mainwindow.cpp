#include "mainwindow.h"
#include "ui_mainwindow.h"  // Автоматически сгенерированный файл
#include "ProcessInfo.h"
#include <string>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);  // Инициализация UI
    connect(ui->UpdateButton, &QPushButton::clicked, this, &MainWindow::update_processes);
    process_widget_amount = 0;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update_processes()
{
    std::vector<ProcessInfo> processes = get_process_list();
    int size = bubbleSort(processes);

    if (size > process_widget_amount){
        for (int i=0; i < size - process_widget_amount; i++){
            create_process_widget();
        }
    }

    else if (size < process_widget_amount){
        for (int i=0; i < process_widget_amount - size; i++){
            delete_process_widget();
        }
    }

    process_widget_amount = size;
    for (int i = 0; i < size; i++)
    {
        ProcessInfo p = processes[i];
        QString name_for_print = QString(p.name);
        QString path_for_print = QString(p.path);
        if (path_for_print.length() > 70) {
            path_for_print = path_for_print.left(70);
        }
        if (name_for_print.length() > 30) {
            name_for_print = name_for_print.left(30);
        }
        exe_name_label_buffer[i]->setText(QString(name_for_print));
        exe_path_label_buffer[i]->setText(QString(path_for_print));
        memory_label_buffer[i]->setText(QString::number(p.memoryUsage));
        cpu_label_buffer[i]->setText(QString("cpu"));
    }

}

void MainWindow::delete_process_widget()
{
    QLayoutItem* item = ui->ProcessLayout->itemAt(process_widget_amount + 2);
    QWidget* widget = item->widget();
    if (widget) {
        ui->ProcessLayout->removeWidget(widget);
        widget->deleteLater();
    }

    cpu_label_buffer.pop_back();
    memory_label_buffer.pop_back();
    exe_name_label_buffer.pop_back();
    exe_path_label_buffer.pop_back();
    process_widget_amount -= 1;
}

void MainWindow::create_process_widget()
{
    QWidget *ProcessWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *ExeNameLabel;
    QLabel *ExePathLabel;
    QLabel *MemoryLabel;
    QLabel *CPULabel;
    ProcessWidget = new QWidget(ui -> ScrollWidget);

    ProcessWidget->setStyleSheet(
        "QWidget {"
        "   border: 1px solid #FF0000;"  // Красная рамка толщиной 2px
        "}"
        );

    ProcessWidget->setMinimumSize(QSize(0, 30));
    ProcessWidget->setMaximumSize(QSize(16777215, 20));
    ProcessWidget->setAutoFillBackground(true);
    horizontalLayout = new QHBoxLayout(ProcessWidget);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    ExeNameLabel = new QLabel(ProcessWidget);

    ExeNameLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

    horizontalLayout->addWidget(ExeNameLabel);

    ExePathLabel = new QLabel(ProcessWidget);

    horizontalLayout->addWidget(ExePathLabel);

    MemoryLabel = new QLabel(ProcessWidget);
    MemoryLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

    horizontalLayout->addWidget(MemoryLabel);

    CPULabel = new QLabel(ProcessWidget);
    CPULabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

    // ExeNameLabel->setText(QCoreApplication::translate("MainWindow", ExeName, nullptr));
    // ExePathLabel->setText(QCoreApplication::translate("MainWindow", ExePath, nullptr));
    // MemoryLabel->setText(QCoreApplication::translate("MainWindow", Memory, nullptr));
    // CPULabel->setText(QCoreApplication::translate("MainWindow", CPU, nullptr));

    horizontalLayout->addWidget(CPULabel);

    ui->ProcessLayout->insertWidget(process_widget_amount + 2, ProcessWidget);
    process_widget_amount += 1;

    exe_name_label_buffer.push_back(ExeNameLabel);
    exe_path_label_buffer.push_back(ExePathLabel);
    memory_label_buffer.push_back(MemoryLabel);
    cpu_label_buffer.push_back(CPULabel);
}
