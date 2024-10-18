#include "knobcontrol.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

KnobControl::KnobControl(QWidget *parent)
    : QWidget{parent}, dragging_(false), minimum_value_(0.0), maximum_value_(100.0),
      current_value_(0.0), _spritesheet(nullptr)
{
    setMinimumSize(50, 50);
    setMouseTracking(true);  // Enable mouse tracking to detect hover
}

void KnobControl::setMinimum(double minimum) {
    minimum_value_ = minimum;
    updateImage();
}

void KnobControl::setMaximum(double maximum) {
    maximum_value_ = maximum;
    updateImage();
}

void KnobControl::setValue(double value) {
    double old_value = current_value_;
    current_value_ = value;
    updateImage();

    emit knobChanged(old_value, current_value_);
}

void KnobControl::setSpriteSheet(std::shared_ptr<SpriteSheet> spriteSheet) {
    _spritesheet = spriteSheet;
    updateImage();
}

void KnobControl::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    if(!_spritesheet || _spritesheet->croppedImages().isEmpty()) return;

    QPainter painter(this);

    // Calculate the mapped value and find the corresponding frame
    double mappedValue = (current_value_ - minimum_value_) / (maximum_value_ - minimum_value_);
    int spriteCount = _spritesheet->croppedImages().size();
    int index = qBound(0, qFloor(mappedValue * spriteCount), spriteCount - 1);

    // Draw the sprite image
    if(index >= 0 && index < _spritesheet->croppedImages().size()) {
        QPixmap sprite = _spritesheet->croppedImages()[index];
        painter.drawPixmap(rect(), sprite);
    }
}

void KnobControl::mousePressEvent(QMouseEvent *event) {
    dragging_ = true;
    last_mouse_position_ = event->pos();
    event->accept();
}


void KnobControl::mouseMoveEvent(QMouseEvent *event) {
    if(!dragging_) return;

    // Calculate knob value based on mouse movement
    QPoint delta = event->pos() - last_mouse_position_;
    double offset = delta.x() - delta.y();  // simplified rotation logic

    double range = maximum_value_ - minimum_value_;
    double step  = range * offset / 100.0;

    double newValue = current_value_ + step;
    newValue = qBound(minimum_value_, newValue, maximum_value_);
    setValue(newValue);

    last_mouse_position_ = event->pos();
    event->accept();
}

void KnobControl::mouseReleaseEvent(QMouseEvent *event) {
    dragging_ = false;
    event->accept();
}

void KnobControl::updateImage() {
    update();  // Trigger a repaint
}

void KnobControl::wheelEvent(QWheelEvent *event) {

    // Handle mouse wheel movement
    int delta = event->angleDelta().y();  // Vertical wheel movement
    double range = maximum_value_ - minimum_value_;

    double step = range * 0.01;  // Define how much each wheel step changes the value
    if (delta > 0) {
        setValue(qBound(minimum_value_, current_value_ + step, maximum_value_));  // Scroll up, increase value
    } else {
        setValue(qBound(minimum_value_, current_value_ - step, maximum_value_));  // Scroll down, decrease value
    }

    event->accept();
}

void KnobControl::enterEvent(QEnterEvent * event) {
    Q_UNUSED(event);

    emit hoverEntered();
}

void KnobControl::leaveEvent(QEvent * event) {
    Q_UNUSED(event);

    emit hoverLeft();
}


QPoint KnobControl::centerPoint() const {
    // Calculate and return the center point of the knob control
    return mapToGlobal(QPoint(width() / 2, height() / 2));
}
