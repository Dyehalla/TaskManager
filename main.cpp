#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // Создаем приложение Qt

    MainWindow window;             // Создаем главное окно
    window.show();                 // Показываем окно

    return app.exec();             // Запускаем цикл обработки событий
}