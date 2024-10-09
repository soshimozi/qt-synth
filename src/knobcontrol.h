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
    double minimum() const { return minimum_value_; }

    void setMaximum(double maximum);
    double maximum() const { return maximum_value_; }

    void setValue(double value);
    double value() const { return current_value_; }

    void setSpriteSheet(std::shared_ptr<SpriteSheet> spritesheet);
    SpriteSheet* spriteSheet() const { return _spritesheet.get(); }

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

private:
    void updateImage();

private:
    bool dragging_;
    QPoint last_mouse_position_;

    double minimum_value_;
    double maximum_value_;
    double current_value_;

    std::shared_ptr<SpriteSheet> _spritesheet;

};

#endif // KNOBCONTROL_H
