#include "mainwindow.h"
#include "ProcessInfo.h"
#include "virustotal.h"
#include <vector>
#include <QLabel>
#include <QApplication>

std::vector<ProcessInfo> get_process_list();

int bubbleSort(std::vector<ProcessInfo> &vector);



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;

    // for (int i = 0; i < init_process_amount; i++)
    // {
    //     window.create_process_widget();
    // }



    window.show();

    return app.exec();
}
