#include "mainwindow.h"
#include "ui_mainwindow.h"  // Автоматически сгенерированный файл
#include "ProcessInfo.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);  // Инициализация UI
    connect(ui->UpdateButton, &QPushButton::clicked, this, &MainWindow::update_processes);
    // connect(ui->NetworkPageButton, &QPushButton::clicked, this, [this]() {
    //     ui->stackedWidget->setCurrentIndex(1); // Переключаем на вторую страницу
    // });
    // connect(ui->ProcessPageButton, &QPushButton::clicked, this, [this]() {
    //     ui->stackedWidget->setCurrentIndex(0); // Переключаем на вторую страницу
    // });
    process_widget_amount = 0;
    network_widget_amount = 0;

    ui->ProcessTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ProcessTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    for (int i = 0; i <= 50; i++) ui->ProcessTable->insertRow(0);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update_processes(){
    std::vector<ProcessInfo> processes = get_process_list();
    int size = bubbleSort(processes);
    int i = 0;
    for (ProcessInfo proc : processes){
        ui->ProcessTable->setItem(i, 0, new QTableWidgetItem(QString(proc.name)));
        ui->ProcessTable->setItem(i, 1, new QTableWidgetItem(QString(proc.path)));
        ui->ProcessTable->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(proc.memoryUsage)));
        i++;
    }
}

void MainWindow::resize_columns_to_content(){
    ui->ProcessTable->resizeColumnsToContents();
    ui->ProcessTable->horizontalHeader()->setStretchLastSection(true);
}




