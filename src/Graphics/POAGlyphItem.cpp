#include "POAGlyphItem.h"

#include <QPainter>

namespace {
    constexpr double kLineWidth = 3.0;  ///< Width of the lines forming the X shape.
}

POAGlyphItem::POAGlyphItem(QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
{
}

double POAGlyphItem::size() const
{
    return m_size;
}

QColor POAGlyphItem::color() const
{
    return m_color;
}

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
    double padding = kLineWidth + 2;
    double halfSize = m_size / 2.0 + padding;
    return QRectF(-halfSize, -halfSize, halfSize * 2, halfSize * 2);
}

void POAGlyphItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    Q_UNUSED(pOption)
    Q_UNUSED(pWidget)

    pPainter->setRenderHint(QPainter::Antialiasing);

    QPen pen(m_color);
    pen.setWidthF(kLineWidth);
    pen.setCapStyle(Qt::RoundCap);
    pPainter->setPen(pen);

    double halfSize = m_size / 2.0;

    // Draw X shape (two diagonal lines)
    pPainter->drawLine(QPointF(-halfSize, -halfSize), QPointF(halfSize, halfSize));
    pPainter->drawLine(QPointF(-halfSize, halfSize), QPointF(halfSize, -halfSize));
}
