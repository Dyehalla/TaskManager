#include "mainwindow.h"
#include "ui_mainwindow.h"  // Автоматически сгенерированный файл
#include "ProcessInfo.h"
#include "NetworkPerformanceItem.h"
#include "virustotal.h"
#include "vt_dialog.h"

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

    ui->ProcessTable->setModel(model);
    ui->ProcessTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->ProcessTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ProcessTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->ProcessTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->ProcessTable, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);

    for (int i = 0; i <= 5; i++){
        model->insertColumn(0);
    }
    ui->ProcessTable->setStyleSheet(
        "QTableView::item:hover { background-color: none; }"

        );

    draw_process_table();
}

void MainWindow::vt_check(std::wstring path) {
    VtDialog dialog(this, path);
    dialog.exec();
}


void MainWindow::showContextMenu(const QPoint& pos) {
    QModelIndex index = ui->ProcessTable->indexAt(pos);
    if (!index.isValid()) return;

    QMenu menu;
    QAction* action1 = menu.addAction("Посмотреть в проводнике");
    QAction* action2 = menu.addAction("Завершить процесс");
    QAction* action3 = menu.addAction("VirusTotal");

    QAction* selected = menu.exec(ui->ProcessTable->viewport()->mapToGlobal(pos));
    if (selected == action1) {
        std::wstring path = path_vector[index.row()];
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(QString(path)).absolutePath()));
    } else if (selected == action2) {
        // TerminateProcessById(model-> index.row())
    } else if (selected == action3) {
        vt_check(path_vector[index.row()]);
    }
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    model = new NonEditableModel;
    ui->setupUi(this);  // Инициализация UI
    connect(ui->UpdateButton, &QPushButton::clicked, this, &MainWindow::update_button);
    connect(ui->NetworkPageButton, &QPushButton::clicked, this, &MainWindow::draw_network_table);
    connect(ui->ProcessPageButton, &QPushButton::clicked, this, &MainWindow::draw_process_table);

    init_table();

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update_table_rows_amount(int new_rows_count){
    int rows_count = model->rowCount();
    if (rows_count < new_rows_count){
        model -> insertRows(rows_count - 1, new_rows_count - rows_count);
    } else if (rows_count > new_rows_count) {
        model->removeRows(new_rows_count, rows_count - new_rows_count);
    }
}

void MainWindow::update_processes(){
    std::vector<ProcessInfo> processes = get_process_list();
    update_table_rows_amount(processes.size());
    ui->ProcessTable->setUpdatesEnabled(false); // отключаем сигналы чтоб кути хернёй маялся
    int size = bubbleSort(processes);
    int i = 0;
    path_vector.clear();
    for (ProcessInfo &proc : processes){
        path_vector.push_back(proc.path);
        model->setItem(i, 0, new QStandardItem(QString(proc.name)));
        model->setItem(i, 1, new QStandardItem(QString("%1").arg(proc.memoryUsage)));
        model->setItem(i, 2, new QStandardItem(QString("%1").arg(proc.cpuUsage)));
        model->setItem(i, 3, new QStandardItem(QString("%1").arg(proc.pid)));
        i++;
    }
    ui->ProcessTable->setUpdatesEnabled(true);
}

void MainWindow::erase_column(int col){
    ui->ProcessTable->setUpdatesEnabled(false);
    model->horizontalHeaderItem(col)->setText("");
    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem* item = model->item(row, col);
        if (item) {
            item->setText(""); // очищаем текст
        }
    }
    ui->ProcessTable->setUpdatesEnabled(true);
}

void MainWindow::resize_columns_to_content(){
    ui->ProcessTable->resizeColumnsToContents();
    ui->ProcessTable->horizontalHeader()->setStretchLastSection(true);
    int lastColumn = model->columnCount() - 1;
    ui->ProcessTable->setColumnWidth(lastColumn, 50);
}

void MainWindow::update_networks(){
    std::vector<NetworkPerformanceItem> networks =  get_networks_list();
    update_table_rows_amount(networks.size());
    ui->ProcessTable->setUpdatesEnabled(false); // отключаем сигналы чтоб кути хернёй маялся
    int i = 0;
    for (NetworkPerformanceItem &perf : networks){
        model->setItem(i, 0, new QStandardItem(QString(perf.ExeName)));
        model->setItem(i, 1, new QStandardItem(QString("%1").arg(perf.OutboundBandwidth / 1024 / 1024)));
        model->setItem(i, 2, new QStandardItem(QString("%1").arg(perf.InboundBandwidth / 1024 / 1024)));
        model->setItem(i, 3, new QStandardItem(QString::fromStdString(perf.LocalAddress)));
        model->setItem(i, 4, new QStandardItem(QString::fromStdString(perf.RemoteAddress)));
        i++;
    }
    ui->ProcessTable->setUpdatesEnabled(true);
}

void MainWindow::draw_network_table(){
    current_table = NETWORK_TABLE;
    model->setHorizontalHeaderLabels({"Exe name", "Network in", "Network out", "Local IP", "Remote IP"});
    update_networks();
    resize_columns_to_content();
}

void MainWindow::draw_process_table(){
    current_table = PROCESS_TABLE;
    model->setHorizontalHeaderLabels({"Exe name", "Memory", "CPU", "PID", "", ""});
    update_processes();
    erase_column(4);
    resize_columns_to_content();
}

void MainWindow::update_button(){
    if (current_table == PROCESS_TABLE) update_processes();
    else if (current_table == NETWORK_TABLE) update_networks();
}



