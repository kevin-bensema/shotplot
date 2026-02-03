#include "GroupCircleItem.h"

#include <QPainter>

GroupCircleItem::GroupCircleItem(Type type, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_type(type)
{
    // Set color based on type
    switch (m_type)
    {
        case Type::Full:
            m_color = QColor(0, 102, 204);  // Blue
            break;
        case Type::Percent80:
            m_color = QColor(0, 204, 102);  // Green
            break;
        case Type::Percent90:
            m_color = QColor(255, 153, 51); // Orange
            break;
    }
}

GroupCircleItem::~GroupCircleItem() = default;

void GroupCircleItem::setCircle(const QPointF &center, double radius)
{
    prepareGeometryChange();
    m_center = center;
    m_radius = radius;
    setPos(center);
    update();
}

QRectF GroupCircleItem::boundingRect() const
{
    double padding = m_lineWidth + 2;
    double r = m_radius + padding;
    return QRectF(-r, -r, r * 2, r * 2);
}

void GroupCircleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (m_radius <= 0)
    {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);

    // Fill with semi-transparent color
    QColor fillColor = m_color;
    fillColor.setAlpha(m_fillOpacity);
    painter->setBrush(fillColor);

    // Outline
    QPen pen(m_color);
    pen.setWidthF(m_lineWidth);
    painter->setPen(pen);

    // Draw circle (centered at item's position, which is m_center)
    painter->drawEllipse(QPointF(0, 0), m_radius, m_radius);

    // Draw small crosshair at center
    double crossSize = 5.0;
    painter->drawLine(QPointF(-crossSize, 0), QPointF(crossSize, 0));
    painter->drawLine(QPointF(0, -crossSize), QPointF(0, crossSize));
}
