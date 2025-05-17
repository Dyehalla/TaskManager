#include <QDialog>

namespace Ui {
class VtDialog;
}

class VtDialog : public QDialog {
    Q_OBJECT

public:
    VtDialog(QWidget* parent, std::wstring& process_path);
    ~VtDialog();
    std::wstring path;
    Ui::VtDialog* ui;
};
