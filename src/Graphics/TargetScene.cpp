#include "TargetScene.h"
#include "ImpactGlyphItem.h"
#include "POAGlyphItem.h"
#include "GroupCircleItem.h"
#include "ScaleLineItem.h"
#include <Core/ShotGroupDocument.h>

#include <QGraphicsPixmapItem>

namespace
{
    constexpr double kTargetImageZValue = -1000.0;
    constexpr double kImpactGlyphZValue = 100.0;
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
        disconnect(m_pDocument, nullptr, this, nullptr);
    }

    m_pDocument = pDocument;
    
    if (m_pDocument)
    {
        connect(m_pDocument, &ShotGroupDocument::impactsChanged,
                this, &TargetScene::updateFromDocument);
        connect(m_pDocument, &ShotGroupDocument::visualizationSettingsChanged,
                this, &TargetScene::updateFromDocument);
        
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

void TargetScene::setPOAGlyph(const QPointF& position)
{
    if (!m_pPoaGlyph)
    {
        m_pPoaGlyph = new POAGlyphItem();
        m_pPoaGlyph->setZValue(kPOAGlyphZValue);
        addItem(m_pPoaGlyph);
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
        setPOAGlyph(m_pDocument->pointOfAim());
    }
    else
    {
        clearPOAGlyph();
    }

    // Update group circle visibility
    updateGroupCirclesVisibility();
    
    // Update POA visibility
    setPOAVisible(m_pDocument->showPointOfAim() && m_pDocument->hasPointOfAimSet());
}
