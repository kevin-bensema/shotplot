#include "ImpactGlyphItem.h"

#include <QPainter>
#include <QFont>
#include <cmath>

ImpactGlyphItem::ImpactGlyphItem(int shotNumber, double diameter, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_shotNumber(shotNumber)
    , m_diameter(diameter)
    , m_color(Qt::red)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
}

ImpactGlyphItem::~ImpactGlyphItem() = default;

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

void ImpactGlyphItem::setColor(const QColor &color)
{
    m_color = color;
    update();
}

QRectF ImpactGlyphItem::boundingRect() const
{
    double padding = m_lineWidth + 2;
    double radius = m_diameter / 2.0 + padding;
    return QRectF(-radius, -radius, radius * 2, radius * 2);
}

void ImpactGlyphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    double radius = m_diameter / 2.0;
    
    // Set up pen for the segmented circle
    QPen pen(m_color);
    pen.setWidthF(m_lineWidth);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // Draw segmented circle with gaps at N, S, E, W
    // Qt angles: 0 = 3 o'clock, positive = counter-clockwise
    // We want gaps at: 90 (N), 0 (E), 270 (S), 180 (W)
    
    QRectF rect(-radius, -radius, m_diameter, m_diameter);
    
    // Each arc segment spans (90 - gapAngle) degrees
    // Starting after each gap
    double arcSpan = 90.0 - m_gapAngle;
    double halfGap = m_gapAngle / 2.0;
    
    // Arc 1: NE quadrant (from ~95 to ~175 degrees) - after N gap, before W gap
    painter->drawArc(rect, (90 + halfGap) * 16, arcSpan * 16);
    
    // Arc 2: NW quadrant (from ~185 to ~265 degrees) - after W gap, before S gap
    painter->drawArc(rect, (180 + halfGap) * 16, arcSpan * 16);
    
    // Arc 3: SW quadrant (from ~275 to ~355 degrees) - after S gap, before E gap
    painter->drawArc(rect, (270 + halfGap) * 16, arcSpan * 16);
    
    // Arc 4: SE quadrant (from ~5 to ~85 degrees) - after E gap, before N gap
    painter->drawArc(rect, (0 + halfGap) * 16, arcSpan * 16);

    // Draw shot number in center
    QString text = QString::number(m_shotNumber);
    
    QFont font = painter->font();
    font.setBold(true);
    double fontSize = qMax(8.0, m_diameter * 0.4);
    font.setPixelSize(static_cast<int>(fontSize));
    painter->setFont(font);

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
    painter->setPen(outlinePen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(textPath);
    
    // Fill with white
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawPath(textPath);
}
