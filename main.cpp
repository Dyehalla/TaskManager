#include <QApplication>
#include <QLabel>
#include "mainwindow.h"
#include <vector>
#include "ProcessInfo.h"


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

    // window.update_processes();

    window.draw_process_table();

    window.show();

    return app.exec();
}
