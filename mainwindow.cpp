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
#include <QFileIconProvider>
#define PROCESS_TABLE_COL_COUNT 4
#define NETWORK_TABLE_COL_COUNT 6
#include <QFontDatabase>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    model = new NonEditableModel;
    table = new TaskManagerTableView;
    ui->setupUi(this);

    int fontId = QFontDatabase::addApplicationFont(":/assets/UbuntuMono-Regular.ttf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.empty()) {
            QFont font(fontFamilies.at(0), 20);
            table->setFont(font);
            ui->label->setFont(font);
            ui->label_2->setFont(font);
            ui->ChangeApiKey->setFont(font);
            ui->changeTimer->setFont(font);
            font.setPointSize(17);
            table->horizontalHeader()->setFont(font);
        }
    }

    ui->centralwidget->layout()->addWidget(table);

    ToggleButton *toggleBtn = new ToggleButton(this);
    toggleBtn->setCursor(Qt::PointingHandCursor);
    ui->TableSwitchLayout->insertWidget(1, toggleBtn);

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

void show_map(std::unordered_map<int, std::vector<DWORD>> map){
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
    while (rows_count < new_rows_count){
        model->insertRow(rows_count);
        for (int i = 0; i < model->columnCount(); i++){
            model->setItem(rows_count, i, new QStandardItem());
        }
        rows_count++;
    }
    while (rows_count > new_rows_count) {
        model->removeRow(rows_count);
        rows_count--;
    }
}

void MainWindow::update_processes(){
    pid_map.clear();
    QFileIconProvider iconProvider;
    std::vector<ProcessInfo> processes = get_process_list();
    update_table_rows_amount(processes.size());
    table->setUpdatesEnabled(false);
    bubbleSortProc(processes, process_sort);
    int i = 0;
    path_vector.clear();
    for (ProcessInfo &proc : processes){
        path_vector.push_back(proc.path);
        QIcon icon = iconProvider.icon(QFileInfo(QString(proc.path)));
        QStandardItem *item = new QStandardItem(QString(proc.name));
        item->setIcon(icon); // Ставим иконку
        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        model->setItem(i, 0, item);
        int mem_usage = proc.memoryUsage;
        QString mem_str;
        if (mem_usage > 1000){
            mem_str = QString("%1.%2 Мб").arg(mem_usage / 1000).arg(mem_usage % 1000 / 10);
        } else mem_str = QString("%1 Кб").arg(mem_usage);
        model->item(i, 1)->setText(mem_str);
        model->item(i, 2)->setText(format_mseconds(proc.cpuUsage));
        if (proc.pid.size() == 1){
            pid_map[i] = {proc.pid[0]};
            model->item(i, 3)->setText(QString("%1").arg(proc.pid[0]));
        } else {
            pid_map[i] = {};
            for (DWORD pid : proc.pid){
                pid_map[i].push_back(pid);
            }
            model->item(i, 3)->setText("");
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
    QFileIconProvider iconProvider;
    std::vector<NetworkPerformanceItem> networks =  get_networks_list();
    bubble_sort_net(networks, network_sort);
    update_table_rows_amount(networks.size());
    table->setUpdatesEnabled(false);
    int i = 0;
    path_vector.clear();
    for (NetworkPerformanceItem &perf : networks){
        path_vector.push_back(perf.ExePath);
        QIcon icon = iconProvider.icon(QFileInfo(QString(perf.ExePath)));
        QStandardItem *item = new QStandardItem(QString(perf.ExeName));
        item->setIcon(icon); // Ставим иконку
        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        model->setItem(i, 0, item);
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
        model->item(i, 1)->setText(outbound_str);
        model->item(i, 2)->setText(inbound_str);
        model->item(i, 3)->setText(QString::fromStdString(perf.LocalAddress));
        model->item(i, 4)->setText(QString::fromStdString(perf.RemoteAddress));
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
            for (int i = 0; i < NETWORK_TABLE_COL_COUNT - PROCESS_TABLE_COL_COUNT; i++) model -> removeColumn(PROCESS_TABLE_COL_COUNT + 1);
        }

        if (!process_col_width.empty()) {
            network_col_width.clear();
            for (int i = 0; i < NETWORK_TABLE_COL_COUNT; i++){
                network_col_width.push_back(table->columnWidth(i));
            }
        }

        current_table = PROCESS_TABLE;
        model->setHorizontalHeaderLabels({"Имя", "Память", "Время ЦП", "PID", ""});
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


