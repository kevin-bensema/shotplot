#include "CentroidGlyphItem.h"
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QPainter>

namespace {
    constexpr double kLineWidth = 3.0;  ///< Width of the lines forming the + shape.
}

CentroidGlyphItem::CentroidGlyphItem(double diameterPixels, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , m_size(diameterPixels)
    , m_service(qx::GetService<GraphicsSettingsService>())
{
}

double CentroidGlyphItem::size() const
{
    return m_size;
}

QColor CentroidGlyphItem::color() const
{
    return m_service.color(GraphicsSettingsService::ColorRole::Centroid);
}

void CentroidGlyphItem::setSize(double size)
{
    prepareGeometryChange();
    m_size = size;
    update();
}

QRectF CentroidGlyphItem::boundingRect() const
{
    double padding = kLineWidth + 2;
    double halfSize = m_size / 2.0 + padding;
    return QRectF(-halfSize, -halfSize, halfSize * 2, halfSize * 2);
}

void CentroidGlyphItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    Q_UNUSED(pOption)
    Q_UNUSED(pWidget)

    pPainter->setRenderHint(QPainter::Antialiasing);

    const QColor color = m_service.color(GraphicsSettingsService::ColorRole::Centroid);

    QPen pen(color);
    pen.setWidthF(kLineWidth);
    pen.setCapStyle(Qt::RoundCap);
    pPainter->setPen(pen);

    double halfSize = m_size / 2.0;

    // Draw + shape (horizontal and vertical lines)
    pPainter->drawLine(QPointF(-halfSize, 0), QPointF(halfSize, 0));  // Horizontal
    pPainter->drawLine(QPointF(0, -halfSize), QPointF(0, halfSize));  // Vertical
}
