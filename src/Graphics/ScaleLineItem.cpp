#include "ScaleLineItem.h"
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QPainter>
#include <cmath>

namespace {
    const double kLineWidth = 3.0;            // Width of the line stroke in pixels
    const double kEndpointRadius = 5.0;       // Radius of the circular endpoint markers
}

ScaleLineItem::ScaleLineItem(QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , m_service(qx::GetService<GraphicsSettingsService>())
    , m_lineWidth(kLineWidth)
    , m_endpointRadius(kEndpointRadius)
{
}

ScaleLineItem::~ScaleLineItem() = default;

void ScaleLineItem::setLine(const QPointF& start, const QPointF& end)
{
    prepareGeometryChange();
    m_start = start;
    m_end = end;
    update();
}

void ScaleLineItem::setStartPoint(const QPointF& start)
{
    prepareGeometryChange();
    m_start = start;
    update();
}

void ScaleLineItem::setEndPoint(const QPointF& end)
{
    prepareGeometryChange();
    m_end = end;
    update();
}

QPointF ScaleLineItem::startPoint() const
{
    return m_start;
}

QPointF ScaleLineItem::endPoint() const
{
    return m_end;
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

void ScaleLineItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    Q_UNUSED(pOption)
    Q_UNUSED(pWidget)

    pPainter->setRenderHint(QPainter::Antialiasing);

    const QColor color = m_service.color(GraphicsSettingsService::ColorRole::CalibrationLine);

    QPen pen(color);
    pen.setWidthF(m_lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    pPainter->setPen(pen);

    // Draw line
    pPainter->drawLine(m_start, m_end);

    // Draw endpoints as filled circles
    pPainter->setBrush(color);
    pPainter->drawEllipse(m_start, m_endpointRadius, m_endpointRadius);
    pPainter->drawEllipse(m_end, m_endpointRadius, m_endpointRadius);
}
