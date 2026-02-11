#include "POAGlyphItem.h"
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QPainter>

namespace {
    constexpr double kLineWidth = 3.0;  ///< Width of the lines forming the X shape.
}

POAGlyphItem::POAGlyphItem(double diameterPixels, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , m_size(diameterPixels)
    , m_service(qx::GetService<GraphicsSettingsService>())
{
}

double POAGlyphItem::size() const
{
    return m_size;
}

QColor POAGlyphItem::color() const
{
    return m_service.color(GraphicsSettingsService::ColorRole::PointOfAim);
}

void POAGlyphItem::setSize(double size)
{
    prepareGeometryChange();
    m_size = size;
    update();
}

void POAGlyphItem::setColor(const QColor& color)
{
    // Deprecated - color now managed by GraphicsSettingsService
    Q_UNUSED(color)
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

    const QColor color = m_service.color(GraphicsSettingsService::ColorRole::PointOfAim);

    QPen pen(color);
    pen.setWidthF(kLineWidth);
    pen.setCapStyle(Qt::RoundCap);
    pPainter->setPen(pen);

    double halfSize = m_size / 2.0;

    // Draw X shape (two diagonal lines)
    pPainter->drawLine(QPointF(-halfSize, -halfSize), QPointF(halfSize, halfSize));
    pPainter->drawLine(QPointF(-halfSize, halfSize), QPointF(halfSize, -halfSize));
}
