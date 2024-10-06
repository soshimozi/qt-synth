#include "tooltip.h"
#include <qboxlayout.h>

#include <QTimer>

ToolTip::ToolTip(QWidget *parent)
    : QWidget{parent}
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);


    // Tooltip style
    setStyleSheet("background-color: rgba(185, 185, 185, 255); border: 1px solid black; border-radius: 8px");

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet("color: black; padding: 8px; font-family: 'Afacad Flux'");


    // QFont font("Afacad Flux");
    // font.setBold(true);
    // label->setFont(font);


    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    setLayout(layout);

    // Create the fade-in and fade-out animations
    m_animation = new QPropertyAnimation(this, "windowOpacity");
    m_animation->setDuration(300);  // Fast fade-in

    connect(m_animation, &QPropertyAnimation::finished, this, [=]() {
        if(windowOpacity() == 0) {
            hide();
            emit tooltipHidden();
        }
    });

}

void ToolTip::showTooltip(const QString& text, const QPoint& position, const double duration) {
    m_label->setText(text);

    adjustSize();
    move(position);
    setWindowOpacity(0);
    show();

    if(m_animation->state() == QAbstractAnimation::Running)
    {
        m_animation->stop();
    }

    // Fade in quickly
    m_animation->setDuration(100);
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(1.0);
    m_animation->start();

    QTimer::singleShot(duration, this, [=]() { fadeOut(); });
}

void ToolTip::fadeOut() {
    m_animation->setDuration(500);
    m_animation->setStartValue(1.0);
    m_animation->setEndValue(0.0);
    m_animation->start();
}
