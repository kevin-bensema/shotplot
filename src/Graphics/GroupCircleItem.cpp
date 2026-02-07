#include "GroupCircleItem.h"
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QPainter>

namespace
{
    constexpr double kLineWidth = 2.0;
    constexpr double kCrossSize = 5.0;

    GraphicsSettingsService::ColorRole typeToColorRole(GroupCircleItem::Type type)
    {
        switch (type)
        {
            case GroupCircleItem::Type::Full:
                return GraphicsSettingsService::ColorRole::FullGroupCircle;
            case GroupCircleItem::Type::Percent80:
                return GraphicsSettingsService::ColorRole::Percent80Circle;
            case GroupCircleItem::Type::Percent90:
                return GraphicsSettingsService::ColorRole::Percent90Circle;
        }
        return GraphicsSettingsService::ColorRole::FullGroupCircle;
    }

    GraphicsSettingsService::OpacityRole typeToOpacityRole(GroupCircleItem::Type type)
    {
        switch (type)
        {
            case GroupCircleItem::Type::Full:
                return GraphicsSettingsService::OpacityRole::FullGroupCircle;
            case GroupCircleItem::Type::Percent80:
                return GraphicsSettingsService::OpacityRole::Percent80Circle;
            case GroupCircleItem::Type::Percent90:
                return GraphicsSettingsService::OpacityRole::Percent90Circle;
        }
        return GraphicsSettingsService::OpacityRole::FullGroupCircle;
    }
}

GroupCircleItem::GroupCircleItem(Type type, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , m_type(type)
    , m_service(qx::GetService<GraphicsSettingsService>())
{
}

GroupCircleItem::Type GroupCircleItem::circleType() const
{
    return m_type;
}

QPointF GroupCircleItem::circleCenter() const
{
    return m_center;
}

double GroupCircleItem::radius() const
{
    return m_radius;
}

void GroupCircleItem::setCircle(const QPointF& center, double radius)
{
    prepareGeometryChange();
    m_center = center;
    m_radius = radius;
    setPos(center);
    update();
}

QRectF GroupCircleItem::boundingRect() const
{
    double padding = kLineWidth + 2;
    double r = m_radius + padding;
    return QRectF(-r, -r, r * 2, r * 2);
}

void GroupCircleItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    Q_UNUSED(pOption)
    Q_UNUSED(pWidget)

    if (m_radius <= 0)
    {
        return;
    }

    pPainter->setRenderHint(QPainter::Antialiasing);

    // Get color and opacity from service
    const QColor color = m_service.color(typeToColorRole(m_type));
    const int opacity = m_service.opacity(typeToOpacityRole(m_type));

    // Fill with semi-transparent color (convert opacity from 0-100 to 0-255)
    QColor fillColor = color;
    fillColor.setAlpha(opacity * 255 / 100);
    pPainter->setBrush(fillColor);

    // Outline
    QPen pen(color);
    pen.setWidthF(kLineWidth);
    pPainter->setPen(pen);

    // Draw circle (centered at item's position, which is m_center)
    pPainter->drawEllipse(QPointF(0, 0), m_radius, m_radius);

    // Draw small crosshair at center
    pPainter->drawLine(QPointF(-kCrossSize, 0), QPointF(kCrossSize, 0));
    pPainter->drawLine(QPointF(0, -kCrossSize), QPointF(0, kCrossSize));
}
