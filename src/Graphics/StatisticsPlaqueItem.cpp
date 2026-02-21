#include "StatisticsPlaqueItem.h"

#include <Core/ShotGroupDocument.h>
#include <Core/UnitConverter.h>
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
#include <QSettings>
#include <QFontMetrics>

namespace
{
    const QString kDisplayUnitsKey = QStringLiteral("View.Display.Units");
    constexpr const char* kDefaultUnitString = "inches";
    constexpr int kTitleFontSizeOffset = 2;      // Points larger than default
    constexpr int kMinImpactCountForStats = 2;
}

StatisticsPlaqueItem::StatisticsPlaqueItem(ShotGroupDocument* pDocument, QGraphicsItem* pParent)
    : QGraphicsObject(pParent)
    , m_pDocument(pDocument)
    , m_service(qx::GetService<GraphicsSettingsService>())
{
    setAcceptHoverEvents(true);
    setZValue(300);

    updateFromDocument();
}

void StatisticsPlaqueItem::setDocument(ShotGroupDocument* pDocument)
{
    m_pDocument = pDocument;
    updateFromDocument();
}

void StatisticsPlaqueItem::updateFromDocument()
{
    if (!m_pDocument)
        return;

    const auto config = m_pDocument->plaqueConfig();

    // Rebuild fonts from config
    m_bodyFont = QFont();
    m_bodyFont.setPointSize(config.baseFontSize);
    m_titleFont = m_bodyFont;
    m_titleFont.setPointSize(config.baseFontSize + kTitleFontSizeOffset);
    m_titleFont.setBold(true);

    // Recompute title bar height from font metrics
    QFontMetrics titleFm(m_titleFont);
    m_titleBarHeight = titleFm.height() + kTitleBarPadding * 2;

    prepareGeometryChange();
    setPos(config.x, config.y);
    m_width = config.width;
    m_height = config.height;

    // Apply opacity from service (0-100 scale -> 0.0-1.0)
    double opacity = m_service.opacity(GraphicsSettingsService::OpacityRole::Plaque) / 100.0;
    setOpacity(opacity);

    update();
}

QRectF StatisticsPlaqueItem::boundingRect() const
{
    return QRectF(0, 0, m_width, m_height);
}

QRectF StatisticsPlaqueItem::titleBarRect() const
{
    return QRectF(0, 0, m_width, m_titleBarHeight);
}

QRectF StatisticsPlaqueItem::bodyRect() const
{
    return QRectF(0, m_titleBarHeight, m_width, m_height - m_titleBarHeight);
}

QRectF StatisticsPlaqueItem::resizeHandleRect() const
{
    return QRectF(m_width - kResizeHandleSize, m_height - kResizeHandleSize,
                  kResizeHandleSize, kResizeHandleSize);
}

QStringList StatisticsPlaqueItem::formatStatistics() const
{
    if (!m_pDocument)
        return {};

    const auto config = m_pDocument->plaqueConfig();
    const Statistics& stats = m_pDocument->statistics();

    // Get display unit preference
    QSettings settings;
    QString unitStr = settings.value(kDisplayUnitsKey, kDefaultUnitString).toString();
    UnitConverter::Unit unit = UnitConverter::unitFromString(unitStr);

    double ppi = m_pDocument->pixelsPerInch();
    double distanceYards = m_pDocument->targetDistance();
    if (m_pDocument->distanceUnit() == ShotGroupDocument::DistanceUnit::Meters)
    {
        distanceYards = UnitConverter::metersToYards(distanceYards);
    }

    bool hasEnoughData = m_pDocument->impactCount() >= kMinImpactCountForStats
                         && m_pDocument->hasScaleFactorSet();

    auto formatValue = [&](double pixels) -> QString
    {
        double inches = UnitConverter::pixelsToInches(pixels, ppi);
        double value = inches;

        if (unit == UnitConverter::Unit::MOA)
        {
            value = UnitConverter::inchesToMOA(inches, distanceYards);
        }
        else if (unit == UnitConverter::Unit::MRAD)
        {
            value = UnitConverter::inchesToMRAD(inches, distanceYards);
        }

        return UnitConverter::formatWithUnit(value, unit);
    };

    auto formatOffset = [&](double pixels) -> QString
    {
        double inches = UnitConverter::pixelsToInches(pixels, ppi);
        double value = inches;

        if (unit == UnitConverter::Unit::MOA)
        {
            value = UnitConverter::inchesToMOA(inches, distanceYards);
        }
        else if (unit == UnitConverter::Unit::MRAD)
        {
            value = UnitConverter::inchesToMRAD(inches, distanceYards);
        }

        return UnitConverter::formatWithUnit(value, unit);
    };

    QStringList lines;

    using PS = ShotGroupDocument::PlaqueStat;

    if (config.enabledStats.contains(PS::ShotCount))
    {
        lines << QStringLiteral("Shots: ") + QString::number(m_pDocument->impactCount());
    }

    if (!hasEnoughData)
    {
        // Show placeholders for stats that need calculation
        if (config.enabledStats.contains(PS::FullGroup))
            lines << QStringLiteral("Full Group: --");
        if (config.enabledStats.contains(PS::Group80))
            lines << QStringLiteral("80% Group: --");
        if (config.enabledStats.contains(PS::Group90))
            lines << QStringLiteral("90% Group: --");
        if (config.enabledStats.contains(PS::MeanRadius))
            lines << QStringLiteral("Mean Radius: --");
        if (config.enabledStats.contains(PS::StdDev))
            lines << QStringLiteral("Std Dev: --");
        if (config.enabledStats.contains(PS::OffsetX))
            lines << QStringLiteral("Offset X: --");
        if (config.enabledStats.contains(PS::OffsetY))
            lines << QStringLiteral("Offset Y: --");
        return lines;
    }

    if (config.enabledStats.contains(PS::FullGroup) && stats.fullGroupCircle.isValid())
    {
        lines << QStringLiteral("Full Group: ") + formatValue(stats.fullGroupCircle.diameterPixels());
    }

    if (config.enabledStats.contains(PS::Group80) && stats.group80Circle.isValid())
    {
        lines << QStringLiteral("80% Group: ") + formatValue(stats.group80Circle.diameterPixels());
    }

    if (config.enabledStats.contains(PS::Group90) && stats.group90Circle.isValid())
    {
        lines << QStringLiteral("90% Group: ") + formatValue(stats.group90Circle.diameterPixels());
    }

    if (config.enabledStats.contains(PS::MeanRadius))
    {
        lines << QStringLiteral("Mean Radius: ") + formatValue(stats.meanRadiusPixels);
    }

    if (config.enabledStats.contains(PS::StdDev))
    {
        lines << QStringLiteral("Std Dev: ") + formatValue(stats.standardDeviationPixels);
    }

    if (stats.offsetFromPOA.has_value())
    {
        const QPointF& offset = stats.offsetFromPOA.value();
        // Image coordinates: right=+X, down=+Y
        // Shooter perspective: right=+X, up=+Y (negate Y)
        if (config.enabledStats.contains(PS::OffsetX))
        {
            lines << QStringLiteral("Offset X: ") + formatOffset(offset.x());
        }
        if (config.enabledStats.contains(PS::OffsetY))
        {
            lines << QStringLiteral("Offset Y: ") + formatOffset(-offset.y());
        }
    }
    else
    {
        if (config.enabledStats.contains(PS::OffsetX))
            lines << QStringLiteral("Offset X: --");
        if (config.enabledStats.contains(PS::OffsetY))
            lines << QStringLiteral("Offset Y: --");
    }

    return lines;
}

void StatisticsPlaqueItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget)
{
    Q_UNUSED(pOption)
    Q_UNUSED(pWidget)

    pPainter->setRenderHint(QPainter::Antialiasing);

    const QColor titleBgColor = m_service.color(GraphicsSettingsService::ColorRole::PlaqueTitleBarBackground);
    const QColor titleTextColor = m_service.color(GraphicsSettingsService::ColorRole::PlaqueTitleBarText);
    const QColor bodyBgColor = m_service.color(GraphicsSettingsService::ColorRole::PlaqueBodyBackground);
    const QColor bodyTextColor = m_service.color(GraphicsSettingsService::ColorRole::PlaqueBodyText);

    // Draw title bar background
    QRectF titleRect = titleBarRect();
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(titleBgColor);
    pPainter->drawRect(titleRect);

    // Draw title text
    pPainter->setFont(m_titleFont);
    pPainter->setPen(titleTextColor);
    QRectF titleTextRect = titleRect.adjusted(kTitleBarPadding, 0, -kTitleBarPadding, 0);
    QString titleText = m_pDocument ? m_pDocument->plaqueConfig().title : QString();
    QFontMetrics titleFm(m_titleFont);
    QString elidedTitle = titleFm.elidedText(titleText, Qt::ElideRight,
                                              static_cast<int>(titleTextRect.width()));
    pPainter->drawText(titleTextRect, Qt::AlignVCenter | Qt::AlignLeft, elidedTitle);

    // Draw body background
    QRectF body = bodyRect();
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(bodyBgColor);
    pPainter->drawRect(body);

    // Draw statistics lines
    pPainter->setFont(m_bodyFont);
    pPainter->setPen(bodyTextColor);
    QFontMetrics bodyFm(m_bodyFont);
    int lineHeight = bodyFm.height();
    int availableWidth = static_cast<int>(body.width()) - kBodyPadding * 2;

    QStringList statLines = formatStatistics();
    int y = static_cast<int>(body.top()) + kBodyPadding;

    for (const QString& line : statLines)
    {
        if (y + lineHeight > static_cast<int>(body.bottom()) - kBodyPadding)
            break;  // No more room

        QString elidedLine = bodyFm.elidedText(line, Qt::ElideRight, availableWidth);
        pPainter->drawText(QRectF(body.left() + kBodyPadding, y, availableWidth, lineHeight),
                           Qt::AlignVCenter | Qt::AlignLeft, elidedLine);
        y += lineHeight;
    }

    // Draw resize handle (small triangular grip in bottom-right)
    QRectF handleRect = resizeHandleRect();
    QPainterPath gripPath;
    gripPath.moveTo(handleRect.bottomRight());
    gripPath.lineTo(handleRect.topRight());
    gripPath.lineTo(handleRect.bottomLeft());
    gripPath.closeSubpath();

    QColor gripColor = titleBgColor;
    gripColor.setAlpha(180);
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(gripColor);
    pPainter->drawPath(gripPath);
}

void StatisticsPlaqueItem::mousePressEvent(QGraphicsSceneMouseEvent* pEvent)
{
    if (pEvent->button() != Qt::LeftButton)
    {
        pEvent->ignore();
        return;
    }

    QPointF localPos = pEvent->pos();

    if (resizeHandleRect().contains(localPos))
    {
        m_dragMode = DragMode::Resize;
        m_dragStartPos = pEvent->scenePos();
        m_dragStartWidth = m_width;
        m_dragStartHeight = m_height;
        pEvent->accept();
    }
    else if (titleBarRect().contains(localPos))
    {
        m_dragMode = DragMode::Move;
        m_dragStartPos = pEvent->scenePos();
        m_dragStartItemPos = pos();
        pEvent->accept();
    }
    else
    {
        pEvent->ignore();
    }
}

void StatisticsPlaqueItem::mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent)
{
    if (m_dragMode == DragMode::None)
    {
        pEvent->ignore();
        return;
    }

    QPointF delta = pEvent->scenePos() - m_dragStartPos;

    if (m_dragMode == DragMode::Move)
    {
        setPos(m_dragStartItemPos + delta);
    }
    else if (m_dragMode == DragMode::Resize)
    {
        prepareGeometryChange();
        m_width = qMax(kMinWidth, m_dragStartWidth + static_cast<int>(delta.x()));
        m_height = qMax(kMinHeight, m_dragStartHeight + static_cast<int>(delta.y()));
    }

    pEvent->accept();
    update();
}

void StatisticsPlaqueItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent)
{
    if (m_dragMode == DragMode::None)
    {
        pEvent->ignore();
        return;
    }

    // Write back updated position and size to document
    if (m_pDocument)
    {
        auto config = m_pDocument->plaqueConfig();
        config.x = static_cast<int>(pos().x());
        config.y = static_cast<int>(pos().y());
        config.width = m_width;
        config.height = m_height;
        m_pDocument->setPlaqueConfig(config);
    }

    m_dragMode = DragMode::None;
    pEvent->accept();
}

void StatisticsPlaqueItem::hoverMoveEvent(QGraphicsSceneHoverEvent* pEvent)
{
    QPointF localPos = pEvent->pos();

    if (resizeHandleRect().contains(localPos))
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if (titleBarRect().contains(localPos))
    {
        setCursor(Qt::SizeAllCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}
