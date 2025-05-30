#include "mainwindow.h"
#include "ProcessInfo.h"
#include "virustotal.h"
#include <vector>
#include <QLabel>
#include <QApplication>
#include <QLoggingCategory>
std::vector<ProcessInfo> get_process_list();

int bubbleSort(std::vector<ProcessInfo> &vector);


int main(int argc, char *argv[])
{
    // QLoggingCategory::setFilterRules("*.debug=false\n"
    //                                  "*.info=false\n"
    //                                  "*.warning=false\n"
    //                                  "*.critical=true"); // Оставляем только критические ошибки
    QApplication app(argc, argv);
    QApplication::setOrganizationName("Academy of VibeCoding");
    QCoreApplication::setApplicationName("ShitManager");
    MainWindow window;

    window.show();

    return app.exec();
}
