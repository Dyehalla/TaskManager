#include <QTableView>
#include <QHeaderView>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QFont>
#include <QPainter>

class TableItemDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        opt.font = QFont("Calibri", 13, -1);
        if (index.column() == 0) {
            opt.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        } else {
            opt.displayAlignment = Qt::AlignCenter | Qt::AlignVCenter;
        }
        QStyledItemDelegate::paint(painter, opt, index);
    }
};


class TaskManagerTableView : public QTableView
{
    Q_OBJECT
public:
    TaskManagerTableView(QWidget *parent = nullptr) : QTableView(parent)
    {
        setItemDelegate(new TableItemDelegate(this));
        setStyleSheet(
            "QTableView::item {padding-left: 15px}"
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

        verticalHeader()->setDefaultSectionSize(32);
        verticalHeader()->setVisible(false);

        QHeaderView *horizontalHeader = this->horizontalHeader();
        horizontalHeader->setMinimumSectionSize(70);
        horizontalHeader->setSectionResizeMode(QHeaderView::Interactive);
        horizontalHeader->setStretchLastSection(true);
        horizontalHeader->setHighlightSections(false);
        horizontalHeader->setMinimumHeight(45);
        horizontalHeader->setStyleSheet(
            "QHeaderView::section {"
            "   background-color: white;"
            "   color: black;"
            // "   padding: 12px 12px;"  // Увеличенные отступы
            "   border-right: 1px solid #e0e0e0;"
            "   border-bottom: 1px solid #e0e0e0;"
            "   font-size: 13pt;"  // Увеличенный размер шрифта
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
        horizontalHeader->setDefaultAlignment(Qt::AlignCenter| Qt::AlignVCenter);
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
