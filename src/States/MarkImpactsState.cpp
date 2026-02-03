#include "MarkImpactsState.h"
#include "Core/ShotGroupDocument.h"
#include "Core/ShotImpact.h"
#include "Widgets/TargetView.h"
#include "Graphics/TargetScene.h"
#include "Commands/AddImpactCommand.h"
#include "Commands/ClearImpactsCommand.h"

#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QUndoStack>

MarkImpactsState::MarkImpactsState(ShotGroupDocument *document, TargetView *view, 
                                   TargetScene *scene, QUndoStack *undoStack)
    : WorkflowState(document, view)
    , m_view(view)
    , m_scene(scene)
    , m_undoStack(undoStack)
{
}

MarkImpactsState::~MarkImpactsState() = default;

void MarkImpactsState::onEnter()
{
    // Connect to document changes for updating circles
    connect(m_document, &ShotGroupDocument::impactsChanged,
            this, &MarkImpactsState::updateGroupCircles);
    
    // Recreate impact glyphs from document
    m_scene->clearImpactGlyphs();
    double diameter = bulletDiameterPixels();
    
    for (const auto &impact : m_document->impacts())
    {
        m_scene->addImpactGlyph(impact.id, impact.position(), diameter);
    }
    
    updateGroupCircles();
}

void MarkImpactsState::onExit()
{
    disconnect(m_document, &ShotGroupDocument::impactsChanged,
               this, &MarkImpactsState::updateGroupCircles);
}

void MarkImpactsState::handleMouseClick(const QPointF &scenePos)
{
    // Create new impact
    int id = m_document->nextImpactId();
    ShotImpact impact(id, scenePos.x(), scenePos.y());
    
    // Use undo command
    m_undoStack->push(new AddImpactCommand(m_document, m_scene, impact, bulletDiameterPixels()));
}

void MarkImpactsState::handleRightClick(const QPointF &scenePos)
{
    // Find and remove impact at this position
    // For now, simple distance-based hit test
    double hitRadius = bulletDiameterPixels() / 2.0;
    
    const auto impacts = m_document->impacts();
    for (int i = 0; i < impacts.size(); ++i) {
        QPointF impactPos = impacts[i].position();
        double dx = scenePos.x() - impactPos.x();
        double dy = scenePos.y() - impactPos.y();
        double dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist <= hitRadius) {
            // Remove this impact
            // TODO: Use RemoveImpactCommand for undo support
            m_scene->removeImpactGlyph(impacts[i].id);
            m_document->removeImpact(i);
            break;
        }
    }
}

bool MarkImpactsState::isComplete() const
{
    return m_document && m_document->impactCount() >= 1;
}

QCursor MarkImpactsState::cursor() const
{
    // Segmented circle cursor - for now use crosshair
    // TODO: Create custom cursor scaled to bullet diameter
    return QCursor(Qt::CrossCursor);
}

void MarkImpactsState::populateToolbar(QToolBar *toolbar)
{
    toolbar->addWidget(new QLabel(tr("Mark all impacts")));
    
    toolbar->addSeparator();
    
    QPushButton *clearButton = new QPushButton(tr("Clear All"));
    connect(clearButton, &QPushButton::clicked, this, &MarkImpactsState::onClearImpacts);
    toolbar->addWidget(clearButton);
}

void MarkImpactsState::onClearImpacts()
{
    if (m_document->impactCount() > 0)
    {
        m_undoStack->push(new ClearImpactsCommand(m_document, m_scene));
    }
}

void MarkImpactsState::updateGroupCircles()
{
    if (m_document->impactCount() < 2)
    {
        m_scene->clearGroupCircles();
        return;
    }
    
    const Statistics &stats = m_document->statistics();
    
    if (stats.fullGroupCircle.isValid())
    {
        m_scene->setFullGroupCircle(stats.fullGroupCircle.center, stats.fullGroupCircle.radiusPixels);
    }
    
    if (stats.group80Circle.isValid())
    {
        m_scene->set80GroupCircle(stats.group80Circle.center, stats.group80Circle.radiusPixels);
    }
    
    if (stats.group90Circle.isValid())
    {
        m_scene->set90GroupCircle(stats.group90Circle.center, stats.group90Circle.radiusPixels);
    }
    
    // Update visibility based on document settings
    m_scene->setGroupCirclesVisible(
        m_document->showFullGroupCircle(),
        m_document->show80PercentCircle(),
        m_document->show90PercentCircle()
    );
}

double MarkImpactsState::bulletDiameterPixels() const
{
    if (!m_document || !m_document->hasScaleFactorSet())
    {
        return 20.0;  // Default fallback
    }
    return m_document->bulletDiameter() * m_document->pixelsPerInch();
}
