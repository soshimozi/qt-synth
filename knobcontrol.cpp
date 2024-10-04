#include "knobcontrol.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

KnobControl::KnobControl(QWidget *parent)
    : QWidget{parent}, m_isDragging(false), m_minimum(0.0), m_maximum(100.0),
      m_value(0.0), m_spriteSheet(nullptr)
{
    setMinimumSize(50, 50);
    setMouseTracking(true);  // Enable mouse tracking to detect hover
}

void KnobControl::setMinimum(double minimum) {
    m_minimum = minimum;
    updateImage();
}

void KnobControl::setMaximum(double maximum) {
    m_maximum = maximum;
    updateImage();
}

void KnobControl::setValue(double value) {
    double old_value = m_value;
    m_value = value;
    updateImage();

    emit knobChanged(old_value, m_value);
}

void KnobControl::setSpriteSheet(std::shared_ptr<SpriteSheet> spriteSheet) {
    m_spriteSheet = spriteSheet;
    updateImage();
}

void KnobControl::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    if(!m_spriteSheet || m_spriteSheet->croppedImages().isEmpty()) return;

    QPainter painter(this);

    // Calculate the mapped value and find the corresponding frame
    double mappedValue = (m_value - m_minimum) / (m_maximum - m_minimum);
    int spriteCount = m_spriteSheet->croppedImages().size();
    int index = qBound(0, qFloor(mappedValue * spriteCount), spriteCount - 1);

    // Draw the sprite image
    if(index >= 0 && index < m_spriteSheet->croppedImages().size()) {
        QPixmap sprite = m_spriteSheet->croppedImages()[index];
        painter.drawPixmap(rect(), sprite);
    }
}

void KnobControl::mousePressEvent(QMouseEvent *event) {
    m_isDragging = true;
    m_lastMousePosition = event->pos();
    event->accept();
}


void KnobControl::mouseMoveEvent(QMouseEvent *event) {
    if(!m_isDragging) return;

    // Calculate knob value based on mouse movement
    QPoint delta = event->pos() - m_lastMousePosition;
    double offset = delta.x() - delta.y();  // simplified rotation logic

    double range = m_maximum - m_minimum;
    double step  = range * offset / 100.0;

    double newValue = m_value + step;
    newValue = qBound(m_minimum, newValue, m_maximum);
    setValue(newValue);

    m_lastMousePosition = event->pos();
    event->accept();
}

void KnobControl::mouseReleaseEvent(QMouseEvent *event) {
    m_isDragging = false;
    event->accept();
}

void KnobControl::updateImage() {
    update();  // Trigger a repaint
}

void KnobControl::wheelEvent(QWheelEvent *event) {

    // Handle mouse wheel movement
    int delta = event->angleDelta().y();  // Vertical wheel movement
    double range = m_maximum - m_minimum;

    double step = range * 0.01;  // Define how much each wheel step changes the value
    if (delta > 0) {
        setValue(qBound(m_minimum, m_value + step, m_maximum));  // Scroll up, increase value
    } else {
        setValue(qBound(m_minimum, m_value - step, m_maximum));  // Scroll down, decrease value
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
