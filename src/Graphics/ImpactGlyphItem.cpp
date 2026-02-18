#include "ImpactGlyphItem.h"
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QPainter>
#include <QFont>
#include <cmath>

namespace {
    constexpr double kLineWidth = 2.0;      ///< Width of the circle outline in pixels
    constexpr double kGapAngle = 15.0;      ///< Angular size of each gap in degrees
}

ImpactGlyphItem::ImpactGlyphItem(int shotNumber, double diameter, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , m_shotNumber(shotNumber)
    , m_diameter(diameter)
    , m_service(qx::GetService<GraphicsSettingsService>())
{
}

void ImpactGlyphItem::setShotNumber(int number)
{
    m_shotNumber = number;
    update();
}

void ImpactGlyphItem::setDiameter(double diameter)
{
    prepareGeometryChange();
    m_diameter = diameter;
    update();
}

void ImpactGlyphItem::setColor(const QColor& color)
{
    // Deprecated - color now managed by GraphicsSettingsService
    Q_UNUSED(color)
}

int ImpactGlyphItem::shotNumber() const
{
    return m_shotNumber;
}

double ImpactGlyphItem::diameter() const
{
    return m_diameter;
}

QColor ImpactGlyphItem::color() const
{
    return m_service.color(GraphicsSettingsService::ColorRole::Impact);
}

QRectF ImpactGlyphItem::boundingRect() const
{
    double padding = kLineWidth + 2;
    double radius = m_diameter / 2.0 + padding;
    return QRectF(-radius, -radius, radius * 2, radius * 2);
}

void ImpactGlyphItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    Q_UNUSED(pOption)
    Q_UNUSED(pWidget)

    pPainter->setRenderHint(QPainter::Antialiasing);

    double radius = m_diameter / 2.0;
    
    const QColor color = m_service.color(GraphicsSettingsService::ColorRole::Impact);
    
    // Set up pen for the segmented circle
    QPen pen(color);
    pen.setWidthF(kLineWidth);
    pen.setCapStyle(Qt::FlatCap);
    pPainter->setPen(pen);
    pPainter->setBrush(Qt::NoBrush);

    // Draw segmented circle with gaps at N, S, E, W
    // Qt angles: 0 = 3 o'clock, positive = counter-clockwise
    // We want gaps at: 90 (N), 0 (E), 270 (S), 180 (W)
    
    QRectF rect(-radius, -radius, m_diameter, m_diameter);
    
    // Each arc segment spans (90 - gapAngle) degrees
    // Starting after each gap
    double arcSpan = 90.0 - kGapAngle;
    double halfGap = kGapAngle / 2.0;
    
    // Arc 1: NE quadrant (from ~95 to ~175 degrees) - after N gap, before W gap
    pPainter->drawArc(rect, (90 + halfGap) * 16, arcSpan * 16);
    
    // Arc 2: NW quadrant (from ~185 to ~265 degrees) - after W gap, before S gap
    pPainter->drawArc(rect, (180 + halfGap) * 16, arcSpan * 16);
    
    // Arc 3: SW quadrant (from ~275 to ~355 degrees) - after S gap, before E gap
    pPainter->drawArc(rect, (270 + halfGap) * 16, arcSpan * 16);
    
    // Arc 4: SE quadrant (from ~5 to ~85 degrees) - after E gap, before N gap
    pPainter->drawArc(rect, (0 + halfGap) * 16, arcSpan * 16);

    // Draw shot number in center
    QString text = QString::number(m_shotNumber);
    
    QFont font = pPainter->font();
    font.setBold(true);
    double fontSize = qMax(8.0, m_diameter * 0.4);
    font.setPixelSize(static_cast<int>(fontSize));
    pPainter->setFont(font);

    // White text with dark outline for visibility
    QPainterPath textPath;
    QFontMetricsF fm(font);
    QRectF textRect = fm.boundingRect(text);
    
    // Center the text
    double textX = -textRect.width() / 2.0;
    double textY = fm.ascent() / 2.0 - fm.descent() / 2.0;
    
    textPath.addText(textX, textY, font, text);
    
    // Draw outline
    QPen outlinePen(Qt::black);
    outlinePen.setWidthF(2.0);
    pPainter->setPen(outlinePen);
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawPath(textPath);
    
    // Fill with white
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(Qt::white);
    pPainter->drawPath(textPath);
}
