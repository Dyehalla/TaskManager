#include <QTableView>
#include <QHeaderView>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QFont>

class TableItemDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        opt.font = QFont("Calibri", 11, -1);
        // Настройка выравнивания
        if (index.column() == 0) {
            opt.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        } else {
            opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        }

        QStyledItemDelegate::paint(painter, opt, index);
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(48); // Увеличенная высота строки
        return size;
    }
};

class TaskManagerTableView : public QTableView
{
    Q_OBJECT
public:
    TaskManagerTableView(QWidget *parent = nullptr) : QTableView(parent)
    {
        // Устанавливаем кастомный делегат
        setItemDelegate(new TableItemDelegate(this));

        // Основные настройки таблицы
        setStyleSheet(
            "QTableView {"
            "   border: 1px solid #e0e0e0;"
            "   background-color: white;"
            "   gridline-color: #f0f0f0;"
            "   font-size: 12pt;"  // Увеличенный размер шрифта
            "   border-radius: 8px;"  // Закругленные углы
            "}"
            "QTableView::item {"
            "   padding: 12px 10px;"  // Увеличенные отступы
            "   border-bottom: 1px solid #f0f0f0;"
            "}"
            "QTableView::item:hover {"
            "   background-color: #f8f8f8;"
            "}"
            "QTableView::item:selected {"
            "   background-color: #e0e0e0;"
            "   color: black;"
            "}"
            "QScrollBar:vertical {"
            "   border: none;"
            "   background: #f5f5f5;"
            "   width: 10px;"
            "   margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::handle:vertical {"
            "   background: #c0c0c0;"
            "   min-height: 20px;"
            "   border-radius: 4px;"
            "}"
            );

        verticalHeader()->setDefaultSectionSize(48);
        verticalHeader()->setVisible(false);

        QHeaderView *horizontalHeader = this->horizontalHeader();
        horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
        horizontalHeader->setMinimumSectionSize(120);
        horizontalHeader->setSectionResizeMode(QHeaderView::Interactive);
        horizontalHeader->setStretchLastSection(true);
        horizontalHeader->setHighlightSections(false);

        horizontalHeader->setStyleSheet(
            "QHeaderView::section {"
            "   background-color: white;"
            "   color: black;"
            "   padding: 12px 10px;"  // Увеличенные отступы
            "   border-right: 1px solid #e0e0e0;"
            "   border-bottom: 1px solid #e0e0e0;"
            "   font-size: 12pt;"  // Увеличенный размер шрифта
            "   border-radius: 0px;"  // Убираем закругления у заголовков
            "}"

            "QHeaderView::section:hover {"
            "   background-color: #f0f0f0;"  // Цвет при наведении
            "}"
            );

        setShowGrid(false);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setAlternatingRowColors(false);
        setMouseTracking(true);
        setWordWrap(false);
        setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    }

protected:

    void mouseMoveEvent(QMouseEvent *event) override
    {
        QModelIndex index = indexAt(event->pos());
        if (index.isValid()) {
            selectionModel()->select(index,
                                     QItemSelectionModel::ClearAndSelect |
                                         QItemSelectionModel::Rows);
        }
        QTableView::mouseMoveEvent(event);
    }
};
