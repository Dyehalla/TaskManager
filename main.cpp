#include <QApplication>
#include <QLabel>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // Создаем приложение Qt

    MainWindow window;             // Создаем главное окно
    std::vector<QLabel*> exe_name_label_buffer;
    std::vector<QLabel*> exe_path_label_buffer;
    std::vector<QLabel*> memory_label_buffer;
    std::vector<QLabel*> cpu_label_buffer;

    window.create_process_widget("1", "2", "3", "4");
    window.create_process_widget("1", "2", "3", "4");

    window.show();                 // Показываем окно



    return app.exec();             // Запускаем цикл обработки событий
}
