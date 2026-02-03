#include "POAGlyphItem.h"

#include <QPainter>

POAGlyphItem::POAGlyphItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

POAGlyphItem::~POAGlyphItem() = default;

void POAGlyphItem::setSize(double size)
{
    prepareGeometryChange();
    m_size = size;
    update();
}

void POAGlyphItem::setColor(const QColor &color)
{
    m_color = color;
    update();
}

QRectF POAGlyphItem::boundingRect() const
{
    double padding = m_lineWidth + 2;
    double halfSize = m_size / 2.0 + padding;
    return QRectF(-halfSize, -halfSize, halfSize * 2, halfSize * 2);
}

void POAGlyphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(m_color);
    pen.setWidthF(m_lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    double halfSize = m_size / 2.0;

    // Draw X shape (two diagonal lines)
    painter->drawLine(QPointF(-halfSize, -halfSize), QPointF(halfSize, halfSize));
    painter->drawLine(QPointF(-halfSize, halfSize), QPointF(halfSize, -halfSize));
}
