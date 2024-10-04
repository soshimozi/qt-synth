#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QWidget>
#include <QLabel.h>
#include <QPropertyAnimation>

class ToolTip : public QWidget
{
    Q_OBJECT
public:
    explicit ToolTip(QWidget *parent = nullptr);
    void showTooltip(const QString &text, const QPoint &position, double displayTime = 500);

signals:
    void tooltipShown();
    void tooltipHidden();

private:
    QLabel *m_label;
    QPropertyAnimation *m_animation;

    void fadeOut();
};

#endif // TOOLTIP_H
