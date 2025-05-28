#ifndef CUSTOM_BUTTON_H
#define CUSTOM_BUTTON_H

#include <QAbstractButton>
#include <QPainter>
#include <QPropertyAnimation>
#include <QEasingCurve>

class ToggleButton : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(int sliderPosition READ sliderPosition WRITE setSliderPosition)

public:
    explicit ToggleButton(QWidget *parent = nullptr)
        : QAbstractButton(parent),
        m_sliderPosition(2),
        m_animation(new QPropertyAnimation(this, "sliderPosition", this))
    {
        setCheckable(true);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        // Настройка анимации
        m_animation->setDuration(200);
        m_animation->setEasingCurve(QEasingCurve::InOutQuad);

        // Подключение сигналов
        connect(this, &QAbstractButton::toggled, this, &ToggleButton::updateSliderPosition);
    }

    QSize sizeHint() const override {
        return QSize(60, 30); // Рекомендуемый размер
    }

    int sliderPosition() const { return m_sliderPosition; }
    void setSliderPosition(int pos) {
        m_sliderPosition = pos;
        update(); // Перерисовываем кнопку при изменении позиции
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // 1. Рисуем фон
        drawBackground(p);

        // 2. Рисуем ползунок
        drawSlider(p);

        // 3. Опционально: рисуем текст
        drawText(p);
    }

    void resizeEvent(QResizeEvent *e) override {
        QAbstractButton::resizeEvent(e);
        updateSliderPosition(isChecked());
    }

private:
    void drawBackground(QPainter &p) {
        QRectF bgRect(1, 1, width()-2, height()-2);
        QColor bgColor = isChecked() ? m_checkedColor : m_uncheckedColor;

        p.setPen(Qt::NoPen);
        p.setBrush(bgColor);
        p.drawRoundedRect(bgRect, height()/2, height()/2);
    }

    void drawSlider(QPainter &p) {
        int sliderSize = height() - 4;
        QRectF sliderRect(m_sliderPosition, 2, sliderSize, sliderSize);

        p.setPen(Qt::NoPen);
        p.setBrush(m_sliderColor);
        p.drawEllipse(sliderRect);
    }

    void drawText(QPainter &p) {
        if (!text().isEmpty()) {
            p.setPen(m_textColor);
            p.setFont(font());

            // Сдвигаем текст в зависимости от положения ползунка
            QRect textRect = rect().adjusted(5, 0, -5, 0);
            if (isChecked()) {
                p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
            } else {
                p.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, text());
            }
        }
    }

    void updateSliderPosition(bool checked) {
        m_animation->stop();
        m_animation->setStartValue(m_sliderPosition);
        m_animation->setEndValue(checked ? width() - height() + 2 : 2);
        m_animation->start();
    }

private:
    int m_sliderPosition;
    QPropertyAnimation *m_animation;

    // Цвета можно сделать настраиваемыми через свойства
    QColor m_uncheckedColor = QColor("#e0e0e0");
    QColor m_checkedColor = QColor("#4CAF50");
    QColor m_sliderColor = Qt::white;
    QColor m_textColor = Qt::black;
};


#endif
