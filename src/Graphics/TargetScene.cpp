#include "TargetScene.h"
#include "ImpactGlyphItem.h"
#include "POAGlyphItem.h"
#include "GroupCircleItem.h"
#include "ScaleLineItem.h"
#include "Core/ShotGroupDocument.h"

#include <QGraphicsPixmapItem>

TargetScene::TargetScene(QObject *parent)
    : QGraphicsScene(parent)
{
}

TargetScene::~TargetScene() = default;

void TargetScene::setTargetImage(const QImage &image)
{
    clearTargetImage();
    
    if (image.isNull())
    {
        return;
    }
    
    m_targetImageItem = addPixmap(QPixmap::fromImage(image));
    m_targetImageItem->setZValue(-1000);  // Below everything else
    
    // Set scene rect to image size
    setSceneRect(image.rect());
}

void TargetScene::clearTargetImage()
{
    if (m_targetImageItem)
    {
        removeItem(m_targetImageItem);
        delete m_targetImageItem;
        m_targetImageItem = nullptr;
    }
}

void TargetScene::setDocument(ShotGroupDocument *doc)
{
    m_document = doc;
    
    if (m_document)
    {
        connect(m_document, &ShotGroupDocument::impactsChanged,
                this, &TargetScene::updateFromDocument);
        connect(m_document, &ShotGroupDocument::visualizationSettingsChanged,
                this, &TargetScene::updateFromDocument);
    }
}

ImpactGlyphItem *TargetScene::addImpactGlyph(int id, const QPointF &position, double diameterPixels)
{
    auto *glyph = new ImpactGlyphItem(id, diameterPixels);
    glyph->setPos(position);
    glyph->setZValue(100);  // Above image, below UI overlays
    addItem(glyph);
    m_impactGlyphs[id] = glyph;
    return glyph;
}

void TargetScene::removeImpactGlyph(int id)
{
    if (m_impactGlyphs.contains(id))
    {
        auto *glyph = m_impactGlyphs.take(id);
        removeItem(glyph);
        delete glyph;
    }
}

void TargetScene::clearImpactGlyphs()
{
    for (auto *glyph : m_impactGlyphs)
    {
        removeItem(glyph);
        delete glyph;
    }
    m_impactGlyphs.clear();
}

void TargetScene::updateImpactGlyphSizes(double diameterPixels)
{
    for (auto *glyph : m_impactGlyphs)
    {
        glyph->setDiameter(diameterPixels);
    }
}

void TargetScene::setPOAGlyph(const QPointF &position)
{
    if (!m_poaGlyph)
    {
        m_poaGlyph = new POAGlyphItem();
        m_poaGlyph->setZValue(90);
        addItem(m_poaGlyph);
    }
    m_poaGlyph->setPos(position);
    m_poaGlyph->setVisible(true);
}

void TargetScene::clearPOAGlyph()
{
    if (m_poaGlyph)
    {
        removeItem(m_poaGlyph);
        delete m_poaGlyph;
        m_poaGlyph = nullptr;
    }
}

void TargetScene::setPOAVisible(bool visible)
{
    if (m_poaGlyph)
    {
        m_poaGlyph->setVisible(visible);
    }
}

void TargetScene::setFullGroupCircle(const QPointF &center, double radius)
{
    if (!m_fullGroupCircle)
    {
        m_fullGroupCircle = new GroupCircleItem(GroupCircleItem::Type::Full);
        m_fullGroupCircle->setZValue(50);
        addItem(m_fullGroupCircle);
    }
    m_fullGroupCircle->setCircle(center, radius);
}

void TargetScene::set80GroupCircle(const QPointF &center, double radius)
{
    if (!m_80GroupCircle)
    {
        m_80GroupCircle = new GroupCircleItem(GroupCircleItem::Type::Percent80);
        m_80GroupCircle->setZValue(51);
        addItem(m_80GroupCircle);
    }
    m_80GroupCircle->setCircle(center, radius);
}

void TargetScene::set90GroupCircle(const QPointF &center, double radius)
{
    if (!m_90GroupCircle)
    {
        m_90GroupCircle = new GroupCircleItem(GroupCircleItem::Type::Percent90);
        m_90GroupCircle->setZValue(52);
        addItem(m_90GroupCircle);
    }
    m_90GroupCircle->setCircle(center, radius);
}

void TargetScene::clearGroupCircles()
{
    if (m_fullGroupCircle)
    {
        removeItem(m_fullGroupCircle);
        delete m_fullGroupCircle;
        m_fullGroupCircle = nullptr;
    }
    if (m_80GroupCircle)
    {
        removeItem(m_80GroupCircle);
        delete m_80GroupCircle;
        m_80GroupCircle = nullptr;
    }
    if (m_90GroupCircle)
    {
        removeItem(m_90GroupCircle);
        delete m_90GroupCircle;
        m_90GroupCircle = nullptr;
    }
}

void TargetScene::setGroupCirclesVisible(bool full, bool g80, bool g90)
{
    if (m_fullGroupCircle)
    {
        m_fullGroupCircle->setVisible(full);
    }
    if (m_80GroupCircle)
    {
        m_80GroupCircle->setVisible(g80);
    }
    if (m_90GroupCircle)
    {
        m_90GroupCircle->setVisible(g90);
    }
}

void TargetScene::showScaleLine(const QPointF &start, const QPointF &end)
{
    if (!m_scaleLine)
    {
        m_scaleLine = new ScaleLineItem();
        m_scaleLine->setZValue(200);
        addItem(m_scaleLine);
    }
    m_scaleLine->setLine(start, end);
    m_scaleLine->setVisible(true);
}

void TargetScene::hideScaleLine()
{
    if (m_scaleLine)
    {
        m_scaleLine->setVisible(false);
    }
}

void TargetScene::setScaleLineStart(const QPointF &start)
{
    if (!m_scaleLine)
    {
        m_scaleLine = new ScaleLineItem();
        m_scaleLine->setZValue(200);
        addItem(m_scaleLine);
    }
    m_scaleLine->setLine(start, start);
    m_scaleLine->setVisible(true);
}

void TargetScene::updateScaleLineEnd(const QPointF &end)
{
    if (m_scaleLine)
    {
        m_scaleLine->setEndPoint(end);
    }
}

void TargetScene::updateFromDocument()
{
    if (!m_document)
    {
        return;
    }
    
    // Update group circle visibility
    setGroupCirclesVisible(
        m_document->showFullGroupCircle(),
        m_document->show80PercentCircle(),
        m_document->show90PercentCircle()
    );
    
    // Update POA visibility
    setPOAVisible(m_document->showPointOfAim() && m_document->hasPointOfAimSet());
}
