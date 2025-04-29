#include "mainwindow.h"
#include "ui_mainwindow.h"  // Автоматически сгенерированный файл

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);  // Инициализация UI
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::create_process_widget(const char * ExeName, const char * ExePath, const char * Memory, const char * CPU) {
    QWidget *ProcessWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *ExeNameLabel;
    QLabel *ExePathLabel;
    QLabel *MemoryLabel;
    QLabel *CPULabel;
    ProcessWidget = new QWidget(ui -> ScrollWidget);

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

    ExeNameLabel->setText(QCoreApplication::translate("MainWindow", ExeName, nullptr));
    ExePathLabel->setText(QCoreApplication::translate("MainWindow", ExePath, nullptr));
    MemoryLabel->setText(QCoreApplication::translate("MainWindow", Memory, nullptr));
    CPULabel->setText(QCoreApplication::translate("MainWindow", CPU, nullptr));

    horizontalLayout->addWidget(CPULabel);

    ui->ProcessLayout->insertWidget(2, ProcessWidget);

}
