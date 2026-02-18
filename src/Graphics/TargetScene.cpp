#include "TargetScene.h"
#include "ImpactGlyphItem.h"
#include "POAGlyphItem.h"
#include "CentroidGlyphItem.h"
#include "GroupCircleItem.h"
#include "ScaleLineItem.h"
#include "StatisticsPlaqueItem.h"
#include <Core/ShotGroupDocument.h>

#include <QGraphicsPixmapItem>

namespace
{
    constexpr double kTargetImageZValue = -1000.0;
    constexpr double kImpactGlyphZValue = 100.0;
    constexpr double kCentroidGlyphZValue = 95.0;
    constexpr double kPOAGlyphZValue = 90.0;
    constexpr double kScaleLineZValue = 200.0;

    double groupCircleTypeToZValue(GroupCircle::Type type)
    {
        switch (type)
        {
            case GroupCircle::Type::Full:
                return 50.0;
            case GroupCircle::Type::Percent80:
                return 51.0;
            case GroupCircle::Type::Percent90:
                return 52.0;
        }
        return 50.0;
    }
}

TargetScene::TargetScene(QObject* pParent)
    : QGraphicsScene(pParent)
{
}

void TargetScene::setTargetImage(const QImage& image)
{
    clearTargetImage();
    
    if (image.isNull())
    {
        return;
    }
    
    m_pTargetImageItem = addPixmap(QPixmap::fromImage(image));
    m_pTargetImageItem->setZValue(kTargetImageZValue);
    
    // Set scene rect to image size with padding based on longest edge
    const int longestEdge = std::max(image.width(), image.height());
    const QRectF imageRect = image.rect();
    setSceneRect(imageRect.adjusted(-longestEdge, -longestEdge, longestEdge, longestEdge));
}

void TargetScene::clearTargetImage()
{
    if (m_pTargetImageItem)
    {
        removeItem(m_pTargetImageItem);
        delete m_pTargetImageItem;
        m_pTargetImageItem = nullptr;
    }
}

QGraphicsPixmapItem* TargetScene::targetImageItem() const
{
    return m_pTargetImageItem;
}

void TargetScene::setDocument(ShotGroupDocument* pDocument)
{
    if (m_pDocument)
    {
        m_pDocument->disconnect(this);
    }

    m_pDocument = pDocument;
    
    if (m_pDocument)
    {
        connect(m_pDocument, &ShotGroupDocument::impactsChanged,
                this, &TargetScene::updateFromDocument);
        connect(m_pDocument, &ShotGroupDocument::visualizationSettingsChanged,
                this, &TargetScene::updateFromDocument);
        connect(m_pDocument, &ShotGroupDocument::statisticsChanged,
                this, &TargetScene::updateFromDocument);
        connect(m_pDocument, &ShotGroupDocument::plaqueSettingsChanged,
                this, &TargetScene::updatePlaque);
        connect(m_pDocument, &ShotGroupDocument::statisticsChanged,
                this, &TargetScene::updatePlaque);
        
        if (m_pPlaqueItem)
        {
            m_pPlaqueItem->setDocument(m_pDocument);
        }

        // Initial update
        updateFromDocument();
    }
}

ImpactGlyphItem* TargetScene::addImpactGlyph(int id, const QPointF& position, double diameterPixels)
{
    auto* pGlyph = new ImpactGlyphItem(id, diameterPixels);
    pGlyph->setPos(position);
    pGlyph->setZValue(kImpactGlyphZValue);
    addItem(pGlyph);
    m_impactGlyphs[id] = pGlyph;
    return pGlyph;
}

void TargetScene::removeImpactGlyph(int id)
{
    if (m_impactGlyphs.contains(id))
    {
        auto* pGlyph = m_impactGlyphs.take(id);
        removeItem(pGlyph);
        delete pGlyph;
    }
}

void TargetScene::clearImpactGlyphs()
{
    for (auto* pGlyph : m_impactGlyphs)
    {
        removeItem(pGlyph);
        delete pGlyph;
    }
    m_impactGlyphs.clear();
}

void TargetScene::updateImpactGlyphSizes(double diameterPixels)
{
    for (auto* pGlyph : m_impactGlyphs)
    {
        pGlyph->setDiameter(diameterPixels);
    }
}

void TargetScene::setPOAGlyph(const QPointF& position, double diameterPixels)
{
    if (!m_pPoaGlyph)
    {
        m_pPoaGlyph = new POAGlyphItem(diameterPixels);
        m_pPoaGlyph->setZValue(kPOAGlyphZValue);
        addItem(m_pPoaGlyph);
    }
    else
    {
        m_pPoaGlyph->setSize(diameterPixels);
    }
    m_pPoaGlyph->setPos(position);
    m_pPoaGlyph->setVisible(true);
}

void TargetScene::clearPOAGlyph()
{
    if (m_pPoaGlyph)
    {
        removeItem(m_pPoaGlyph);
        delete m_pPoaGlyph;
        m_pPoaGlyph = nullptr;
    }
}

void TargetScene::setPOAVisible(bool visible)
{
    if (m_pPoaGlyph)
    {
        m_pPoaGlyph->setVisible(visible);
    }
}

void TargetScene::setCentroidGlyph(const QPointF& position, double diameterPixels)
{
    if (!m_pCentroidGlyph)
    {
        m_pCentroidGlyph = new CentroidGlyphItem(diameterPixels);
        m_pCentroidGlyph->setZValue(kCentroidGlyphZValue);
        addItem(m_pCentroidGlyph);
    }
    else
    {
        m_pCentroidGlyph->setSize(diameterPixels);
    }
    m_pCentroidGlyph->setPos(position);
    m_pCentroidGlyph->setVisible(true);
}

void TargetScene::clearCentroidGlyph()
{
    if (m_pCentroidGlyph)
    {
        removeItem(m_pCentroidGlyph);
        delete m_pCentroidGlyph;
        m_pCentroidGlyph = nullptr;
    }
}

void TargetScene::setCentroidVisible(bool visible)
{
    if (m_pCentroidGlyph)
    {
        m_pCentroidGlyph->setVisible(visible);
    }
}

void TargetScene::setGroupCircle(GroupCircle::Type type, const QPointF& center, double radius)
{
    if (!m_groupCircles.contains(type))
    {
        auto* pCircle = new GroupCircleItem(type);
        pCircle->setZValue(groupCircleTypeToZValue(type));
        addItem(pCircle);
        m_groupCircles[type] = pCircle;
    }
    m_groupCircles[type]->setCircle(center, radius);
}

void TargetScene::clearGroupCircles()
{
    for (auto* pCircle : m_groupCircles)
    {
        removeItem(pCircle);
        delete pCircle;
    }
    m_groupCircles.clear();
}

void TargetScene::updateGroupCirclesVisibility()
{
    if (!m_pDocument)
    {
        return;
    }

    for (auto type : GroupCircle::allTypes())
    {
        if (m_groupCircles.contains(type))
        {
            m_groupCircles[type]->setVisible(m_pDocument->showGroupCircle(type));
        }
    }
}

void TargetScene::showScaleLine(const QPointF& start, const QPointF& end)
{
    if (!m_pScaleLine)
    {
        m_pScaleLine = new ScaleLineItem();
        m_pScaleLine->setZValue(kScaleLineZValue);
        addItem(m_pScaleLine);
    }
    m_pScaleLine->setLine(start, end);
    m_pScaleLine->setVisible(true);
}

void TargetScene::hideScaleLine()
{
    if (m_pScaleLine)
    {
        m_pScaleLine->setVisible(false);
    }
}

void TargetScene::setScaleLineStart(const QPointF& start)
{
    if (!m_pScaleLine)
    {
        m_pScaleLine = new ScaleLineItem();
        m_pScaleLine->setZValue(kScaleLineZValue);
        addItem(m_pScaleLine);
    }
    m_pScaleLine->setLine(start, start);
    m_pScaleLine->setVisible(true);
}

void TargetScene::updateScaleLineEnd(const QPointF& end)
{
    if (m_pScaleLine)
    {
        m_pScaleLine->setEndPoint(end);
    }
}

void TargetScene::updateFromDocument()
{
    if (!m_pDocument)
    {
        return;
    }
    
    // Update impact glyphs
    clearImpactGlyphs();
    double diameter = 20.0; // Default
    if (m_pDocument->hasScaleFactorSet())
    {
        diameter = m_pDocument->bulletDiameter() * m_pDocument->pixelsPerInch();
    }
    
    for (const auto& impact : m_pDocument->impacts())
    {
        addImpactGlyph(impact.id, impact.position(), diameter);
    }

    // Update POA glyph
    if (m_pDocument->hasPointOfAimSet())
    {
        // Calculate POA glyph diameter: 1.2x bullet diameter
        double poaDiameter = diameter * 1.2;
        setPOAGlyph(m_pDocument->pointOfAim(), poaDiameter);
    }
    else
    {
        clearPOAGlyph();
    }

    // Update centroid glyph
    const Statistics& stats = m_pDocument->statistics();
    if (stats.valid)
    {
        // Calculate glyph diameter: 1.2x bullet diameter
        double centroidDiameter = diameter * 1.2;
        setCentroidGlyph(stats.centroid, centroidDiameter);
    }
    else
    {
        clearCentroidGlyph();
    }

    // Reconstruct group circles from statistics.
    // This must happen here (not only in MarkImpactsState) so that circles are
    // present when the document is loaded and the app opens directly in
    // VisualizationState, bypassing MarkImpactsState entirely.
    clearGroupCircles();
    if (stats.valid)
    {
        if (stats.fullGroupCircle.isValid())
            setGroupCircle(GroupCircle::Type::Full, stats.fullGroupCircle.center, stats.fullGroupCircle.radiusPixels);
        if (stats.group80Circle.isValid())
            setGroupCircle(GroupCircle::Type::Percent80, stats.group80Circle.center, stats.group80Circle.radiusPixels);
        if (stats.group90Circle.isValid())
            setGroupCircle(GroupCircle::Type::Percent90, stats.group90Circle.center, stats.group90Circle.radiusPixels);
    }

    // Update group circle visibility
    updateGroupCirclesVisibility();
    
    // Update POA visibility
    setPOAVisible(m_pDocument->showPointOfAim() && m_pDocument->hasPointOfAimSet());
    
    // Update centroid visibility
    setCentroidVisible(m_pDocument->showCentroid() && stats.valid);
}

void TargetScene::showPlaque()
{
    if (!m_pDocument)
        return;

    if (!m_pPlaqueItem)
    {
        m_pPlaqueItem = new StatisticsPlaqueItem(m_pDocument);
        addItem(m_pPlaqueItem);
    }

    m_pPlaqueItem->updateFromDocument();

    if (m_pDocument->plaqueConfig().enabled)
    {
        m_pPlaqueItem->setVisible(true);
    }
}

void TargetScene::hidePlaque()
{
    if (m_pPlaqueItem)
    {
        m_pPlaqueItem->setVisible(false);
    }
}

void TargetScene::updatePlaque()
{
    if (m_pPlaqueItem && m_pPlaqueItem->isVisible())
    {
        m_pPlaqueItem->updateFromDocument();
    }
}
