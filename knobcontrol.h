#ifndef KNOBCONTROL_H
#define KNOBCONTROL_H

#include "spritesheet.h"
#include <QWidget>

class KnobControl : public QWidget
{
    Q_OBJECT
public:
    explicit KnobControl(QWidget *parent = nullptr);

    void setMinimum(double minimum);
    double minimum() const { return m_minimum; }

    void setMaximum(double maximum);
    double maximum() const { return m_maximum; }

    void setValue(double value);
    double value() const { return m_value; }

    void setSpriteSheet(std::shared_ptr<SpriteSheet> spritesheet);
    SpriteSheet* spriteSheet() const { return m_spriteSheet.get(); }

    QPoint centerPoint() const;

signals:
    void knobChanged(double oldValue, double newValue);

    void hoverEntered();
    void hoverLeft();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    //void mouseEnterEvent(QEvent *event()) override;

private:
    bool m_isDragging;
    QPoint m_lastMousePosition;

    double m_minimum;
    double m_maximum;
    double m_value;

    std::shared_ptr<SpriteSheet> m_spriteSheet;

    void updateImage();
};

#endif // KNOBCONTROL_H
