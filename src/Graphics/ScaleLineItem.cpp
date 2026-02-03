#include "ScaleLineItem.h"

#include <QPainter>
#include <cmath>

ScaleLineItem::ScaleLineItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

ScaleLineItem::~ScaleLineItem() = default;

void ScaleLineItem::setLine(const QPointF &start, const QPointF &end)
{
    prepareGeometryChange();
    m_start = start;
    m_end = end;
    update();
}

void ScaleLineItem::setStartPoint(const QPointF &start)
{
    prepareGeometryChange();
    m_start = start;
    update();
}

void ScaleLineItem::setEndPoint(const QPointF &end)
{
    prepareGeometryChange();
    m_end = end;
    update();
}

QRectF ScaleLineItem::boundingRect() const
{
    double padding = m_lineWidth + m_endpointRadius + 2;
    
    double minX = qMin(m_start.x(), m_end.x()) - padding;
    double minY = qMin(m_start.y(), m_end.y()) - padding;
    double maxX = qMax(m_start.x(), m_end.x()) + padding;
    double maxY = qMax(m_start.y(), m_end.y()) + padding;
    
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

void ScaleLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(m_color);
    pen.setWidthF(m_lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    // Draw line
    painter->drawLine(m_start, m_end);

    // Draw endpoints as filled circles
    painter->setBrush(m_color);
    painter->drawEllipse(m_start, m_endpointRadius, m_endpointRadius);
    painter->drawEllipse(m_end, m_endpointRadius, m_endpointRadius);
}
