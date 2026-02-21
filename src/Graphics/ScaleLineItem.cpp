#include "ScaleLineItem.h"
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QPainter>

namespace {
    constexpr double kLineWidth = 1.0;
    constexpr double kEndpointRadius = 5.0;
    constexpr int kEndpointOpacity = 50;
}

ScaleLineItem::ScaleLineItem(QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , m_service(qx::GetService<GraphicsSettingsService>())
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
    constexpr double padding = kLineWidth + kEndpointRadius + 2;

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
    pen.setWidthF(kLineWidth);
    pen.setCapStyle(Qt::RoundCap);
    pPainter->setPen(pen);

    // Draw line
    pPainter->drawLine(m_start, m_end);

    // Draw endpoints as transparent circles with solid perimeter and crosshair
    QPen endpointPen(color);
    endpointPen.setWidthF(1.0);
    pPainter->setPen(endpointPen);

    QColor fillColor = color;
    fillColor.setAlpha(kEndpointOpacity);
    pPainter->setBrush(fillColor);

    for (const QPointF& pt : {m_start, m_end})
    {
        pPainter->drawEllipse(pt, kEndpointRadius, kEndpointRadius);
        pPainter->drawLine(QPointF(pt.x() - kEndpointRadius, pt.y()),
                           QPointF(pt.x() + kEndpointRadius, pt.y()));
        pPainter->drawLine(QPointF(pt.x(), pt.y() - kEndpointRadius),
                           QPointF(pt.x(), pt.y() + kEndpointRadius));
    }
}
