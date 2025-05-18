#include "mainwindow.h"
#include "ui_mainwindow.h"  // Автоматически сгенерированный файл
#include "ProcessInfo.h"
#include "NetworkPerformanceItem.h"
#include "virustotal.h"
#include "vt_dialog.h"
#include <QSortFilterProxyModel>

#define PROCESS_TABLE_COL_COUNT 4
#define NETWORK_TABLE_COL_COUNT 6

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    model = new NonEditableModel;
    sort_model = new CustomSortModel(this, &current_table);
    sort_model -> setSourceModel(model);

    ui->setupUi(this);  // Инициализация UI
    ui->ProcessTable->verticalHeader()->setVisible(false);

    connect(ui->UpdateButton, &QPushButton::clicked, this, &MainWindow::update_button);
    connect(ui->NetworkPageButton, &QPushButton::clicked, this, &MainWindow::draw_network_table);
    connect(ui->ProcessPageButton, &QPushButton::clicked, this, &MainWindow::draw_process_table);
    connect(ui->ChangeApiKey, &QPushButton::clicked, this, &MainWindow::set_virustotal_api_key);
    connect(ui->ProcessTable->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::header_click);

    init_table();

}

void MainWindow::init_table(){
    ui->ProcessTable->setModel(sort_model);

    ui->ProcessTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->ProcessTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ProcessTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->ProcessTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->ProcessTable, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);

    ui->ProcessTable->setStyleSheet(
        "QTableView::item:hover { background-color: none; }"

        );

    draw_process_table();
}

void MainWindow::header_click(int column){

    ui->ProcessTable->sortByColumn(column, Qt::DescendingOrder);
}

void MainWindow::vt_check(std::wstring path) {
    VtDialog* dialog = new VtDialog(this, path);
    dialog->show();
    dialog->start_analysis();
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
    ui->ProcessTable->setUpdatesEnabled(false);
    // int size = bubbleSortProc(processes);
    int i = 0;
    path_vector.clear();
    for (ProcessInfo &proc : processes){
        path_vector.push_back(proc.path);
        model->setItem(i, 0, new QStandardItem(QString(proc.name)));
        int mem_usage = proc.memoryUsage;
        QString mem_str;
        if (mem_usage > 1000){
            mem_str = QString("%1.%2 Мб").arg(mem_usage / 1000).arg(mem_usage % 1000 / 10);
        } else mem_str = QString("%1 Кб").arg(mem_usage);
        model->setItem(i, 1, new QStandardItem(mem_str));
        model->setItem(i, 2, new QStandardItem(format_mseconds(proc.cpuUsage)));
        model->setItem(i, 3, new QStandardItem(QString("%1").arg(proc.pid)));
        i++;
    }
    ui->ProcessTable->setUpdatesEnabled(true);
}

void MainWindow::erase_column(int col){
    ui->ProcessTable->setUpdatesEnabled(false);
    // model->horizontalHeaderItem(col)->setText("");
    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem* item = model->item(row, col);
        if (item) {
            item->setText("");
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
    // bubble_sort_net(networks);
    update_table_rows_amount(networks.size());
    ui->ProcessTable->setUpdatesEnabled(false);
    int i = 0;
    path_vector.clear();
    for (NetworkPerformanceItem &perf : networks){
        path_vector.push_back(perf.ExePath);
        model->setItem(i, 0, new QStandardItem(QString(perf.ExeName)));
        long outbound_value = perf.OutboundBandwidth / 1000;
        QString outbound_str;
        if (outbound_value > 1000){
            outbound_str = QString("%1.%2 Мб/c").arg(outbound_value / 1000).arg(outbound_value % 1000 / 10);
        } else outbound_str = QString("%1 Кб/c").arg(outbound_value);
        long inbound_value = perf.InboundBandwidth / 1000;
        QString inbound_str;
        if (inbound_value > 1000){
            inbound_str = QString("%1.%2 Мб/c").arg(inbound_value / 1000).arg(inbound_value % 1000 / 10);
        } else inbound_str = QString("%1 Кб/c").arg(inbound_value);
        model->setItem(i, 1, new QStandardItem(outbound_str));
        model->setItem(i, 2, new QStandardItem(inbound_str));
        model->setItem(i, 3, new QStandardItem(QString::fromStdString(perf.LocalAddress)));
        model->setItem(i, 4, new QStandardItem(QString::fromStdString(perf.RemoteAddress)));
        model->setItem(i, 5, new QStandardItem(QString("%1").arg(perf.ProcessId)));
        i++;
    }
    ui->ProcessTable->setUpdatesEnabled(true);
}

void MainWindow::draw_network_table(){
    if (current_table != NETWORK_TABLE){

        process_col_width.clear();
        for (int i = 0; i < PROCESS_TABLE_COL_COUNT; i++){
            process_col_width.push_back(ui->ProcessTable->columnWidth(i));
        }
        current_table = NETWORK_TABLE;
        model->setHorizontalHeaderLabels({"Exe name", "Network out", "Network in", "Local IP", "Remote IP", "PID", ""});
        if (!network_col_width.empty()){
            for (int i = 0; i < PROCESS_TABLE_COL_COUNT; i++) ui->ProcessTable->setColumnWidth(i, network_col_width[i]);
        } else resize_columns_to_content();
        update_networks();
    }
}

void MainWindow::draw_process_table(){
    if (current_table != PROCESS_TABLE){
        if (model->columnCount() > 5){
            // Удаляю разницу столбцов (считается, что у нетворка всегда больше столбцом, чем у процесса)
            for (int i = 0; i < NETWORK_TABLE_COL_COUNT - PROCESS_TABLE_COL_COUNT; i++) model -> removeColumn(PROCESS_TABLE_COL_COUNT + 1);
        }

        if (!process_col_width.empty()) {
            network_col_width.clear();
            for (int i = 0; i < NETWORK_TABLE_COL_COUNT; i++){
                network_col_width.push_back(ui->ProcessTable->columnWidth(i));
            }
        }

        current_table = PROCESS_TABLE;
        model->setHorizontalHeaderLabels({"Exe name", "Memory", "CPU", "PID", ""});
        update_processes();
        erase_column(4);
        erase_column(5);
        if (!process_col_width.empty()){
            for (int i = 0; i < PROCESS_TABLE_COL_COUNT; i++) ui->ProcessTable->setColumnWidth(i, process_col_width[i]);
        }
        else resize_columns_to_content();
    }
}

void MainWindow::update_button(){
    if (current_table == PROCESS_TABLE) update_processes();
    else if (current_table == NETWORK_TABLE) update_networks();
}

void MainWindow::set_virustotal_api_key(){
    QSettings settings;
    bool ok;
    QString apiKey = QInputDialog::getText(this,
                                           "VirusTotal API Key",
                                           "Введите ваш API ключ для VirusTotal:",
                                           QLineEdit::Normal,
                                           "",
                                           &ok);

    if (!ok || apiKey.isEmpty()) {
        QMessageBox::warning(this,
                             "Ошибка",
                             "API ключ не был введен. Функционал VirusTotal недоступен.");
    }
    // Сохраняем ключ
    settings.setValue("VirusTotal/apiKey", apiKey);

}

int extractLeadingDigits(const QString& str) {
    int result = 0;
    for (const QChar& ch : str) {
        if (ch.isDigit()) {
            result = result * 10 + ch.digitValue();
        } else break;
    }
    return result;
}


