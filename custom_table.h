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
        opt.font = QFont("Calibri", 11, -1);

        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, QColor("#f2f2f2"));
        }

        if (index.column() == 0) {
            // QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
            // icon = QIcon(":/icons/icon.svg");

            // const int iconSize = 24;
            // const int leftMargin = 16;
            // const int spacing = 8;
            // QRect iconRect = opt.rect;
            // iconRect.setLeft(opt.rect.left() + leftMargin);
            // iconRect.setWidth(iconSize);
            // iconRect.setHeight(iconSize);
            // iconRect.moveTop(iconRect.top() + (opt.rect.height() - iconSize) / 2);
            // icon.paint(painter, iconRect, Qt::AlignCenter);
            opt.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;

            QRect textRect = opt.rect;
            // textRect.setLeft(iconRect.right() + spacing);
            textRect.adjust(0, 0, -10, 0);

            painter->save();
            painter->setFont(opt.font);
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, opt.text);
            painter->restore();

        } else {
            opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
            QStyledItemDelegate::paint(painter, opt, index);
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(48);
        return size;
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
