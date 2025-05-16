#include "mainwindow.h"
#include "ui_mainwindow.h"  // Автоматически сгенерированный файл
#include "ProcessInfo.h"
#include "NetworkPerformanceItem.h"
#include <iostream>
#include <QDebug>
#include "virustotal.h"
#include <QThread>
void MainWindow::test(){
    const QString apiKey = "332b1e51ec7ce35a0738543eb468611f333500ed9e39bfe46cd5e75db041b77b";
    const QString filePath = "C:/games/Braid.Anniversary.Edition.v20240603/braid64_d3d11_final.exe";

    // Загружаем файл
    QString analysisId = uploadFileToVirusTotal(networkManager, filePath, apiKey);


    qDebug() << "Analysis ID:" << analysisId;

    // Получаем отчет
    QThread::msleep(5000);
    QJsonObject report = getVirusTotalReport(networkManager, analysisId, apiKey);
    qDebug() << "Report:" << report;
}

void MainWindow::init_table(){
    for (int row = 0; row <= 70; row++){
        ui->ProcessTable->insertRow(0);
        for (int col = 0; col < ui->ProcessTable->columnCount(); ++col)
        {
            // Создаем новый элемент
            QTableWidgetItem* item = new QTableWidgetItem();

            // Устанавливаем флаги, убирая возможность редактирования
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);

            // Вставляем элемент в таблицу
            ui->ProcessTable->setItem(row, col, item);
        }
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);  // Инициализация UI
    connect(ui->UpdateButton, &QPushButton::clicked, this, &MainWindow::test);
    connect(ui->NetworkPageButton, &QPushButton::clicked, this, &MainWindow::draw_network_table);
    connect(ui->ProcessPageButton, &QPushButton::clicked, this, &MainWindow::draw_process_table);

    init_table();

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update_processes(){
    std::vector<ProcessInfo> processes = get_process_list();
    ui->ProcessTable->setUpdatesEnabled(false); // отключаем сигналы чтоб кути хернёй маялся
    int size = bubbleSort(processes);
    int i = 0;
    for (ProcessInfo &proc : processes){
        ui->ProcessTable->setItem(i, 0, new QTableWidgetItem(QString(proc.name)));
        ui->ProcessTable->setItem(i, 1, new QTableWidgetItem(QString(proc.path)));
        ui->ProcessTable->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(proc.memoryUsage)));
        i++;
    }
    ui->ProcessTable->setUpdatesEnabled(true);
}

void MainWindow::erase_column(int col){
    ui->ProcessTable->setUpdatesEnabled(false);
    ui->ProcessTable->horizontalHeaderItem(col)->setText("");
    for (int row = 0; row < ui->ProcessTable->rowCount(); ++row) {
        QTableWidgetItem* item = ui->ProcessTable->item(row, col);
        if (item) {
            item->setText(""); // очищаем текст
            // item->setData(Qt::DisplayRole, QVariant()); // альтернативный способ
        }
    }
    ui->ProcessTable->setUpdatesEnabled(true);
}

void MainWindow::resize_columns_to_content(){
    ui->ProcessTable->resizeColumnsToContents();
    ui->ProcessTable->horizontalHeader()->setStretchLastSection(true);
    int lastColumn = ui->ProcessTable->columnCount() - 1;  // Индекс последнего столбца
    ui->ProcessTable->setColumnWidth(lastColumn, 50);     // Установка ширины 200 пикселей
}

void MainWindow::update_networks(){
    std::vector<NetworkPerformanceItem> networks =  get_networks_list();
    ui->ProcessTable->setUpdatesEnabled(false); // отключаем сигналы чтоб кути хернёй маялся
    int i = 0;for (NetworkPerformanceItem &perf : networks){
        ui->ProcessTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(perf.ExeName)));
        ui->ProcessTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(perf.ExePath)));
        ui->ProcessTable->setItem(i, 2, new QTableWidgetItem(QString("%1").arg(perf.OutboundBandwidth / 1024 / 1024)));
        ui->ProcessTable->setItem(i, 3, new QTableWidgetItem(QString("%1").arg(perf.InboundBandwidth / 1024 / 1024)));
        ui->ProcessTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(perf.LocalAddress)));
        ui->ProcessTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(perf.RemoteAddress)));
        i++;
    }
    ui->ProcessTable->setUpdatesEnabled(true);
}

void MainWindow::draw_network_table(){
    current_table = NETWORK_TABLE;
    ui->ProcessTable->horizontalHeaderItem(2)->setText("Network in");
    ui->ProcessTable->horizontalHeaderItem(3)->setText("Network out");
    ui->ProcessTable->horizontalHeaderItem(4)->setText("Local IP");
    ui->ProcessTable->horizontalHeaderItem(5)->setText("Remote IP");
    update_networks();
    resize_columns_to_content();
}

void MainWindow::draw_process_table(){
    current_table = PROCESS_TABLE;
    ui->ProcessTable->horizontalHeaderItem(2)->setText("Memory");
    ui->ProcessTable->horizontalHeaderItem(3)->setText("cpu");
    update_processes();
    erase_column(4);
    erase_column(5);
    resize_columns_to_content();
}

void MainWindow::update_button(){
    if (current_table == PROCESS_TABLE) update_processes();
    else if (current_table == NETWORK_TABLE) update_networks();
}



