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

    label_ = new QLabel(this);
    label_->setAlignment(Qt::AlignCenter);
    label_->setStyleSheet("color: black; padding: 8px; font-family: 'Afacad Flux'");


    // QFont font("Afacad Flux");
    // font.setBold(true);
    // label->setFont(font);


    auto layout = new QVBoxLayout(this);
    layout->addWidget(label_);
    setLayout(layout);

    // Create the fade-in and fade-out animations
    animation_ = new QPropertyAnimation(this, "windowOpacity");
    animation_->setDuration(300);  // Fast fade-in

    connect(animation_, &QPropertyAnimation::finished, this, [=]() {
        if(windowOpacity() == 0) {
            hide();
            emit tooltipHidden();
        }
    });

}

void ToolTip::showTooltip(const QString& text, const QPoint& position, const double duration) {
    label_->setText(text);

    adjustSize();
    move(position);
    setWindowOpacity(0);
    show();

    if(animation_->state() == QAbstractAnimation::Running)
    {
        animation_->stop();
    }

    // Fade in quickly
    animation_->setDuration(100);
    animation_->setStartValue(0.0);
    animation_->setEndValue(1.0);
    animation_->start();

    QTimer::singleShot(duration, this, [=]() { fadeOut(); });
}

void ToolTip::fadeOut() {
    animation_->setDuration(500);
    animation_->setStartValue(1.0);
    animation_->setEndValue(0.0);
    animation_->start();
}
