#pragma once

#include <QColor>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QWidget>

#include <memory>
#include <vector>

class QPainter;

class PatchPanelWidget;

enum class JackDirection
{
    Input,
    Output
};

class JackBase
{
public:
    JackBase(PatchPanelWidget* panel, QString label, const QRect& rect, JackDirection direction, const QColor& baseColor);
    virtual ~JackBase() = default;

    const QString& label() const noexcept;
    const QRect& rect() const noexcept;
    JackDirection direction() const noexcept;

    QPoint anchor() const;
    bool hitTest(const QPoint& pos) const;

    void setHovered(bool hovered) noexcept;
    bool isHovered() const noexcept;

    virtual bool isConnected() const = 0;
    virtual void draw(QPainter& painter) const;
    virtual void drawConnections(QPainter& painter) const;

protected:
    PatchPanelWidget* panel() const noexcept;
    const QColor& baseColor() const noexcept;

private:
    PatchPanelWidget* m_panel;
    QString m_label;
    QRect m_rect;
    JackDirection m_direction;
    QColor m_baseColor;
    bool m_hovered = false;
};

class OutputJack;

class InputJack : public JackBase
{
public:
    InputJack(PatchPanelWidget* panel, QString label, const QRect& rect, const QColor& baseColor);

    void setSource(OutputJack* source);
    void disconnect();
    OutputJack* source() const noexcept;

    bool isConnected() const override;

private:
    OutputJack* m_source = nullptr;
};

class OutputJack : public JackBase
{
public:
    struct Connection
    {
        InputJack* target = nullptr;
        QColor color;
    };

    OutputJack(PatchPanelWidget* panel, QString label, const QRect& rect, const QColor& baseColor);

    void connectTo(InputJack* input, const QColor& color);
    void removeTarget(InputJack* input);
    void clearConnections();

    const std::vector<Connection>& connections() const noexcept;

    bool isConnected() const override;
    void drawConnections(QPainter& painter) const override;

private:
    std::vector<Connection> m_connections;
};

class PatchPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PatchPanelWidget(QWidget* parent = nullptr);

    InputJack* addInputJack(const QString& label, const QRect& rect,  const QColor& baseColor);
    OutputJack* addOutputJack(const QString& label, const QRect& rect,  const QColor& baseColor);

    static void drawCable(QPainter& painter, QPoint start, QPoint end, const QColor& color, int feedback = 0);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    JackBase* hitTest(const QPoint& pos) const;
    bool canConnect(const JackBase* a, const JackBase* b) const;
    void clearHoverStates();
    QColor nextCableColor();

private:
    std::vector<std::unique_ptr<JackBase>> m_jacks;
    int m_nextCableColorIndex = 0;

    JackBase* m_pressedJack = nullptr;
    QPoint m_pressPos;

    bool m_isDragging = false;
    JackBase* m_dragJack = nullptr;
    JackBase* m_dropTargetJack = nullptr;
    QPoint m_dragPos;
};
