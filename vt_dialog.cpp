#include "vt_dialog.h"
#include "ui_vt_dialog.h" // Автоматически сгенерированный файл

VtDialog::VtDialog(QWidget* parent, std::wstring& process_path) : QDialog(parent), ui(new Ui::VtDialog) {
    ui->setupUi(this); // Загружаем интерфейс из .ui файла
    path = process_path;
}

VtDialog::~VtDialog() {
    delete ui;
}
