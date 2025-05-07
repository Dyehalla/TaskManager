#include <QApplication>
#include <QLabel>
#include "mainwindow.h"
#include <vector>
#include "ProcessInfo.h"


std::vector<ProcessInfo> get_process_list();

int bubbleSort(std::vector<ProcessInfo> &vector)
{
    int size = vector.size();
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (vector[j].memoryUsage < vector[j + 1].memoryUsage)
                std::swap(vector[j], vector[j + 1]);
        }
    }
    return size;
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;

    std::vector<ProcessInfo> processes = get_process_list();
    int init_process_amount = processes.size();

    for (int i = 0; i < init_process_amount; i++)
    {
        window.create_process_widget();
    }

    window.update_processes();
    window.show();

    return app.exec();
}
