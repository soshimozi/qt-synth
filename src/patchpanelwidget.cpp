#include "patchpanelwidget.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>

namespace
{
const std::vector<QColor> kCablePalette =
    {
        QColor(0xcc, 0x00, 0x00),
        QColor(0x00, 0xcc, 0xcc),
        QColor(0xcc, 0x44, 0x00),
        QColor(0x00, 0x88, 0xcc),
        QColor(0xcc, 0x88, 0x00),
        QColor(0x00, 0x44, 0xcc),
        QColor(0xcc, 0xcc, 0x00),
        QColor(0x00, 0x00, 0xcc),
        QColor(0x88, 0xcc, 0x00),
        QColor(0x44, 0x00, 0xcc),
        QColor(0x44, 0xcc, 0x00),
        QColor(0x88, 0x00, 0xcc),
        QColor(0x00, 0xcc, 0x00),
        QColor(0xcc, 0x00, 0xcc),
        QColor(0x00, 0xcc, 0x44),
        QColor(0xcc, 0x00, 0x88),
        QColor(0x00, 0xcc, 0x88),
        QColor(0xcc, 0x00, 0x44)
};

QColor brightenToward200(const QColor& c)
{
    int r = c.red();
    int g = c.green();
    int b = c.blue();

    if (r < 200)
        r += (200 - r) / 3;
    if (g < 200)
        g += (200 - g) / 3;
    if (b < 200)
        b += (200 - b) / 3;

    return QColor(r, g, b);
}
}

// ============================================================
// JackBase
// ============================================================

JackBase::JackBase(PatchPanelWidget* panel, QString label, const QRect& rect, JackDirection direction, const QColor& baseColor)
    : m_panel(panel),
    m_label(std::move(label)),
    m_rect(rect),
    m_direction(direction),
    m_baseColor(baseColor)
{
}

const QString& JackBase::label() const noexcept
{
    return m_label;
}

const QRect& JackBase::rect() const noexcept
{
    return m_rect;
}

JackDirection JackBase::direction() const noexcept
{
    return m_direction;
}

QPoint JackBase::anchor() const
{
    const int x = (m_direction == JackDirection::Input)
    ? m_rect.left() + 12
    : m_rect.right() - 12;

    return QPoint(x, m_rect.center().y());
}

bool JackBase::hitTest(const QPoint& pos) const
{
    return m_rect.contains(pos);
}

void JackBase::setHovered(bool hovered) noexcept
{
    m_hovered = hovered;
}

bool JackBase::isHovered() const noexcept
{
    return m_hovered;
}

void JackBase::drawConnections(QPainter& painter) const
{
    Q_UNUSED(painter);
}

PatchPanelWidget* JackBase::panel() const noexcept
{
    return m_panel;
}

void JackBase::draw(QPainter& painter) const
{
    const QColor& base = m_baseColor;
    const QColor rimColor = m_hovered ? base.lighter(145) : base.lighter(115);
    const QColor centerColor = isConnected() ? base.lighter(130) : base.darker(170);

    if (m_hovered)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 20));
        painter.drawRoundedRect(m_rect.adjusted(-4, -4, 4, 4), 6, 6);
    }

    const int centerY = m_rect.center().y();
    const int centerX = (m_direction == JackDirection::Input)
                            ? m_rect.left() + 12
                            : m_rect.right() - 12;

    const QRectF outerRing(centerX - 10, centerY - 10, 20, 20);
    const QRectF innerRing(centerX - 8, centerY - 8, 16, 16);
    const QRectF hole(centerX - 4.5, centerY - 4.5, 9, 9);

    {
        QPen pen(QColor(20, 20, 20));
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(QColor(55, 55, 60));
        painter.drawEllipse(outerRing);
    }

    {
        QPen pen(rimColor);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.setBrush(QColor(90, 90, 96));
        painter.drawEllipse(innerRing);
    }

    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(centerColor);
        painter.drawEllipse(hole);
    }

    if (isConnected())
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(240, 240, 240, 210));
        painter.drawEllipse(QRectF(centerX - 2, centerY - 2, 4, 4));
    }

    painter.setPen(QColor(225, 225, 230));

    const QRect textRect(centerX - 30, m_rect.bottom() + 2, 60, 14);
    painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, m_label);
}

const QColor& JackBase::baseColor() const noexcept
{
    return m_baseColor;
}

// ============================================================
// InputJack
// ============================================================

InputJack::InputJack(PatchPanelWidget* panel, QString label, const QRect& rect, const QColor& baseColor)
    : JackBase(panel, std::move(label), rect, JackDirection::Input, baseColor)
{
}

void InputJack::setSource(OutputJack* source)
{
    m_source = source;
}

void InputJack::disconnect()
{
    if (m_source != nullptr)
        m_source->removeTarget(this);
}

OutputJack* InputJack::source() const noexcept
{
    return m_source;
}

bool InputJack::isConnected() const
{
    return m_source != nullptr;
}

// ============================================================
// OutputJack
// ============================================================

OutputJack::OutputJack(PatchPanelWidget* panel, QString label, const QRect& rect, const QColor& baseColor)
    : JackBase(panel, std::move(label), rect, JackDirection::Output, baseColor)
{
}

void OutputJack::connectTo(InputJack* input, const QColor& color)
{
    if (input == nullptr)
        return;

    removeTarget(input);

    m_connections.push_back({ input, color });
    input->setSource(this);
}

void OutputJack::removeTarget(InputJack* input)
{
    if (input == nullptr)
        return;

    auto it = std::remove_if(
        m_connections.begin(),
        m_connections.end(),
        [input](const Connection& c)
        {
            return c.target == input;
        });

    const bool removed = (it != m_connections.end());
    m_connections.erase(it, m_connections.end());

    if (removed && input->source() == this)
        input->setSource(nullptr);
}

void OutputJack::clearConnections()
{
    auto copy = m_connections;
    for (const Connection& c : copy)
    {
        if (c.target != nullptr)
            removeTarget(c.target);
    }
}

const std::vector<OutputJack::Connection>& OutputJack::connections() const noexcept
{
    return m_connections;
}

bool OutputJack::isConnected() const
{
    return !m_connections.empty();
}

void OutputJack::drawConnections(QPainter& painter) const
{
    for (const Connection& c : m_connections)
    {
        if (c.target == nullptr)
            continue;

        PatchPanelWidget::drawCable(
            painter,
            anchor(),
            c.target->anchor(),
            c.color,
            0);
    }
}

// ============================================================
// PatchPanelWidget
// ============================================================

PatchPanelWidget::PatchPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(560, 240);
}

InputJack* PatchPanelWidget::addInputJack(const QString& label, const QRect& rect, const QColor& baseColor)
{
    auto jack = std::make_unique<InputJack>(this, label, rect, baseColor);
    InputJack* result = jack.get();
    m_jacks.push_back(std::move(jack));
    update();
    return result;
}

OutputJack* PatchPanelWidget::addOutputJack(const QString& label, const QRect& rect, const QColor& baseColor)
{
    auto jack = std::make_unique<OutputJack>(this, label, rect, baseColor);
    OutputJack* result = jack.get();
    m_jacks.push_back(std::move(jack));
    update();
    return result;
}

QColor PatchPanelWidget::nextCableColor()
{
    if (kCablePalette.empty())
        return QColor(0xcc, 0x00, 0x00);

    const QColor color = kCablePalette[static_cast<std::size_t>(m_nextCableColorIndex)];
    m_nextCableColorIndex = (m_nextCableColorIndex + 1) % static_cast<int>(kCablePalette.size());
    return color;
}

void PatchPanelWidget::drawCable(QPainter& painter, QPoint start, QPoint end, const QColor& color, int feedback)
{
    const qreal mx = (start.x() + end.x()) * 0.5;
    const qreal my = std::max(start.y(), end.y()) + 40.0;

    QPainterPath path;
    path.moveTo(start);
    path.cubicTo(QPointF(mx, my), QPointF(mx, my), end);

    painter.setBrush(Qt::NoBrush);

    QColor c = color;
    for (int i = 3; i >= 1; --i)
    {
        QPen pen(c, i, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.drawPath(path);
        c = brightenToward200(c);
    }

    if (feedback == 1)
    {
        painter.setPen(QPen(QColor(0x00, 0xff, 0x00), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(end, 8, 8);
    }
    else if (feedback == 2)
    {
        painter.setPen(QPen(QColor(0xff, 0x00, 0x00), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawLine(end + QPoint(-8, -8), end + QPoint(8, 8));
        painter.drawLine(end + QPoint(8, -8), end + QPoint(-8, 8));
    }
}

void PatchPanelWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), QColor(32, 32, 36));

    for (const auto& jack : m_jacks)
        jack->drawConnections(painter);

    for (const auto& jack : m_jacks)
        jack->draw(painter);

    if (m_isDragging && m_dragJack != nullptr)
    {
        int feedback = 0;
        if (m_dropTargetJack != nullptr)
            feedback = canConnect(m_dragJack, m_dropTargetJack) ? 1 : 2;

        drawCable(
            painter,
            m_dragJack->anchor(),
            m_dragPos,
            QColor(0x80, 0x80, 0x80),
            feedback);
    }
}

void PatchPanelWidget::mousePressEvent(QMouseEvent* event)
{
    JackBase* hit = hitTest(event->pos());

    if (event->button() == Qt::RightButton)
    {
        if (auto* input = dynamic_cast<InputJack*>(hit))
        {
            input->disconnect();
            update();
        }
        else if (auto* output = dynamic_cast<OutputJack*>(hit))
        {
            output->clearConnections();
            update();
        }
        return;
    }

    if (event->button() != Qt::LeftButton)
        return;

    m_pressedJack = hit;
    m_pressPos = event->pos();

    m_isDragging = false;
    m_dragJack = nullptr;
    m_dropTargetJack = nullptr;

    update();
}

void PatchPanelWidget::mouseMoveEvent(QMouseEvent* event)
{
    clearHoverStates();

    JackBase* hovered = hitTest(event->pos());
    if (hovered != nullptr)
        hovered->setHovered(true);

    if (m_pressedJack != nullptr && !m_isDragging)
    {
        const int dragDistance = (event->pos() - m_pressPos).manhattanLength();
        const bool movedFarEnough = dragDistance >= QApplication::startDragDistance();
        const bool leftSourceJack = !m_pressedJack->rect().contains(event->pos());

        if (movedFarEnough && leftSourceJack)
        {
            m_isDragging = true;
            m_dragJack = m_pressedJack;
            m_dragPos = event->pos();
        }
    }

    if (m_isDragging)
    {
        m_dragPos = event->pos();

        JackBase* over = hitTest(event->pos());
        if (over != nullptr && over != m_dragJack)
        {
            m_dropTargetJack = over;
            m_dropTargetJack->setHovered(true);
        }
        else
        {
            m_dropTargetJack = nullptr;
        }
    }

    update();
}

void PatchPanelWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (m_isDragging && m_dragJack != nullptr)
    {
        JackBase* target = hitTest(event->pos());
        if (target != nullptr && target != m_dragJack && canConnect(m_dragJack, target))
        {
            auto* output = dynamic_cast<OutputJack*>(m_dragJack);
            auto* input = dynamic_cast<InputJack*>(target);

            if (output == nullptr)
            {
                output = dynamic_cast<OutputJack*>(target);
                input = dynamic_cast<InputJack*>(m_dragJack);
            }

            if (output != nullptr && input != nullptr)
            {
                if (input->source() != nullptr && input->source() != output)
                    input->disconnect();

                output->connectTo(input, nextCableColor());
            }
        }
    }

    m_pressedJack = nullptr;
    m_isDragging = false;
    m_dragJack = nullptr;
    m_dropTargetJack = nullptr;

    update();
}

JackBase* PatchPanelWidget::hitTest(const QPoint& pos) const
{
    for (const auto& jack : m_jacks)
    {
        if (jack->hitTest(pos))
            return jack.get();
    }

    return nullptr;
}

bool PatchPanelWidget::canConnect(const JackBase* a, const JackBase* b) const
{
    if (a == nullptr || b == nullptr)
        return false;

    return a->direction() != b->direction();
}

void PatchPanelWidget::clearHoverStates()
{
    for (auto& jack : m_jacks)
        jack->setHovered(false);
}
