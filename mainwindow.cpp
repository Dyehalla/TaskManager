#include "mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Важный вопрос"); 
    resize(400, 300);     

    QLabel *label = new QLabel("Учишь плюсы?");

    label->setFont(QFont("Arial", 24)); 
    label->setAlignment(Qt::AlignCenter);    
    setCentralWidget(label);    
}

MainWindow::~MainWindow()
{
}