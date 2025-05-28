#include "mainwindow.h"
#include "ui_mainwindow.h"  // Автоматически сгенерированный файл
#include "ProcessInfo.h"
#include "NetworkPerformanceItem.h"
#include "virustotal.h"
#include "vt_dialog.h"
#include <unordered_map>
#include "timer_dialog.h"
#include "custom_button.h"
#include "custom_table.h"

#define PROCESS_TABLE_COL_COUNT 4
#define NETWORK_TABLE_COL_COUNT 6

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    model = new NonEditableModel;
    table = new TaskManagerTableView;

    ui->setupUi(this);  // Инициализация UI

    ui->centralwidget->layout()->addWidget(table);

    ToggleButton *toggleBtn = new ToggleButton(this);
    ui->TableSwitchLayout->insertWidget(1, toggleBtn);

    table->verticalHeader()->setVisible(false);

    // connect(ui->UpdateButton, &QPushButton::clicked, this, &MainWindow::update_button);
    connect(ui->ChangeApiKey, &QPushButton::clicked, this, &MainWindow::set_virustotal_api_key);
    connect(ui->changeTimer, &QPushButton::clicked, this, &MainWindow::change_timer);
    connect(table->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::header_click);
    connect(toggleBtn, &QPushButton::clicked, this, &MainWindow::change_table);

    init_table();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        timeout();
    });
    timer->start(refresh_type * 500);
}

void MainWindow::change_table(){
    if (current_table == PROCESS_TABLE) draw_network_table();
    else draw_process_table();
}

void MainWindow::change_timer(){
    TimerDialog dialog(this, &refresh_type);
    dialog.exec();
    timer->stop();
    timer->start(refresh_type * 500);
}

void MainWindow::timeout() {
    if (current_table == PROCESS_TABLE) update_processes();
    else update_networks();
}

void MainWindow::init_table(){
    table->setModel(model);

    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenu);

    table->setStyleSheet(
        "QTableView::item:hover { background-color: none; }"

        );

    draw_process_table();
}

void MainWindow::header_click(int column){

    if (current_table == PROCESS_TABLE){
        if (column == 0) process_sort = 0;
        else if (column == 1) process_sort = 2;
        else if (column == 2) process_sort = 1;
        update_processes();
    } else {
        if (column == 0) network_sort = 0;
        else if (column == 1) network_sort = 1;
        else if (column == 2) network_sort = 2;
        else if (column == 3) network_sort = 3;
        else if (column == 4) network_sort = 4;
        update_networks();
    }
}

void MainWindow::vt_check(std::wstring path) {
    VtDialog* dialog = new VtDialog(this, path);
    dialog->show();
    dialog->start_analysis();
}

void show_ap(std::unordered_map<int, std::vector<DWORD>> map){
    for (std::pair<int, std::vector<DWORD>> elem : map) qDebug() << elem;
}

void MainWindow::showContextMenu(const QPoint& pos) {
    QModelIndex index = table->indexAt(pos);
    if (!index.isValid()) return;

    QMenu menu;
    QAction* action1 = menu.addAction("Посмотреть в проводнике");
    QAction* action2 = menu.addAction("Завершить процесс");
    QAction* action3 = menu.addAction("VirusTotal");

    QAction* selected = menu.exec(table->viewport()->mapToGlobal(pos));
    int row = index.row();
    if (selected == action1) {
        std::wstring path = path_vector[row];
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(QString(path)).absolutePath()));
    } else if (selected == action2) {
        for (int pid : pid_map[row]){
            qDebug() << TerminateProcessById(pid);
        }
    } else if (selected == action3) {
        vt_check(path_vector[row]);
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
    pid_map.clear();
    std::vector<ProcessInfo> processes = get_process_list();
    update_table_rows_amount(processes.size());
    table->setUpdatesEnabled(false);
    bubbleSortProc(processes, process_sort);
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
        if (proc.pid.size() == 1){
            pid_map[i] = {proc.pid[0]};
            model->setItem(i, 3, new QStandardItem(QString("%1").arg(proc.pid[0])));
        } else {
            pid_map[i] = {};
            for (DWORD pid : proc.pid){
                pid_map[i].push_back(pid);
            }
            model->setItem(i, 3, new QStandardItem());
        }
        i++;
    }
    table->setUpdatesEnabled(true);
}

void MainWindow::erase_column(int col){
    table->setUpdatesEnabled(false);
    // model->horizontalHeaderItem(col)->setText("");
    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem* item = model->item(row, col);
        if (item) {
            item->setText("");
        }
    }
    table->setUpdatesEnabled(true);
}

void MainWindow::resize_columns_to_content(){
    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    int lastColumn = model->columnCount() - 1;
    table->setColumnWidth(lastColumn, 50);
}

void MainWindow::update_networks(){
    std::vector<NetworkPerformanceItem> networks =  get_networks_list();
    bubble_sort_net(networks, network_sort);
    update_table_rows_amount(networks.size());
    table->setUpdatesEnabled(false);
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
    table->setUpdatesEnabled(true);
}

void MainWindow::draw_network_table(){
    if (current_table != NETWORK_TABLE){
        process_col_width.clear();
        for (int i = 0; i < PROCESS_TABLE_COL_COUNT; i++){
            process_col_width.push_back(table->columnWidth(i));
        }
        current_table = NETWORK_TABLE;
        model->setHorizontalHeaderLabels({"Имя", "Сеть ↑", "Сеть ↓", "Локальный IP", "Удалённый IP", "PID", ""});
        if (!network_col_width.empty()){
            for (int i = 0; i < PROCESS_TABLE_COL_COUNT; i++) table->setColumnWidth(i, network_col_width[i]);
            update_networks();
        } else {
            update_networks();
            resize_columns_to_content();
        }
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
                network_col_width.push_back(table->columnWidth(i));
            }
        }

        current_table = PROCESS_TABLE;
        model->setHorizontalHeaderLabels({"Exe name", "Memory", "CPU", "PID", ""});
        update_processes();
        erase_column(4);
        erase_column(5);
        if (!process_col_width.empty()){
            for (int i = 0; i < PROCESS_TABLE_COL_COUNT; i++) table->setColumnWidth(i, process_col_width[i]);
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


