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

namespace {
    constexpr double kDefaultBulletDiameterPixels = 20.0;
}

MarkImpactsState::MarkImpactsState(ShotGroupDocument* pDocument, TargetView* pView, 
                                   TargetScene* pScene, QUndoStack* pUndoStack)
    : WorkflowState(pDocument, pView)
    , m_pView(pView)
    , m_pScene(pScene)
    , m_pUndoStack(pUndoStack)
{
}

MarkImpactsState::~MarkImpactsState() = default;

void MarkImpactsState::onEnter()
{
    // Connect to document changes for updating circles
    connect(m_pDocument, &ShotGroupDocument::impactsChanged,
            this, &MarkImpactsState::updateGroupCircles);
    
    // Recreate impact glyphs from document
    m_pScene->clearImpactGlyphs();
    double diameter = bulletDiameterPixels();
    
    for (const auto &impact : m_pDocument->impacts())
    {
        m_pScene->addImpactGlyph(impact.id, impact.position(), diameter);
    }
    
    updateGroupCircles();
}

void MarkImpactsState::onExit()
{
    disconnect(m_pDocument, &ShotGroupDocument::impactsChanged,
               this, &MarkImpactsState::updateGroupCircles);
}

void MarkImpactsState::handleMouseClick(const QPointF &scenePos)
{
    // Create new impact
    int id = m_pDocument->nextImpactId();
    ShotImpact impact(id, scenePos.x(), scenePos.y());
    
    // Use undo command
    m_pUndoStack->push(new AddImpactCommand(m_pDocument, m_pScene, impact, bulletDiameterPixels()));
}

void MarkImpactsState::handleRightClick(const QPointF &scenePos)
{
    // Find and remove impact at this position
    // For now, simple distance-based hit test
    double hitRadius = bulletDiameterPixels() / 2.0;
    
    const auto impacts = m_pDocument->impacts();
    for (int i = 0; i < impacts.size(); ++i) {
        QPointF impactPos = impacts[i].position();
        double dx = scenePos.x() - impactPos.x();
        double dy = scenePos.y() - impactPos.y();
        double dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist <= hitRadius) {
            // Remove this impact
            // TODO: Use RemoveImpactCommand for undo support
            m_pScene->removeImpactGlyph(impacts[i].id);
            m_pDocument->removeImpact(i);
            break;
        }
    }
}

bool MarkImpactsState::isComplete() const
{
    return m_pDocument && m_pDocument->impactCount() >= 1;
}

QCursor MarkImpactsState::cursor() const
{
    // Segmented circle cursor - for now use crosshair
    // TODO: Create custom cursor scaled to bullet diameter
    return QCursor(Qt::CrossCursor);
}

void MarkImpactsState::populateToolbar(QToolBar* pToolbar)
{
    pToolbar->addWidget(new QLabel(tr("Mark all impacts")));
    
    pToolbar->addSeparator();
    
    QPushButton* pClearButton = new QPushButton(tr("Clear All"));
    connect(pClearButton, &QPushButton::clicked, [this]() {
        if (m_pDocument->impactCount() > 0)
        {
            m_pUndoStack->push(new ClearImpactsCommand(m_pDocument, m_pScene));
        }
    });
    pToolbar->addWidget(pClearButton);
}

void MarkImpactsState::updateGroupCircles()
{
    if (m_pDocument->impactCount() < 2)
    {
        m_pScene->clearGroupCircles();
        return;
    }
    
    const Statistics &stats = m_pDocument->statistics();
    
    if (stats.fullGroupCircle.isValid())
    {
        m_pScene->setFullGroupCircle(stats.fullGroupCircle.center, stats.fullGroupCircle.radiusPixels);
    }
    
    if (stats.group80Circle.isValid())
    {
        m_pScene->set80GroupCircle(stats.group80Circle.center, stats.group80Circle.radiusPixels);
    }
    
    if (stats.group90Circle.isValid())
    {
        m_pScene->set90GroupCircle(stats.group90Circle.center, stats.group90Circle.radiusPixels);
    }
    
    // Update visibility based on document settings
    m_pScene->setGroupCirclesVisible(
        m_pDocument->showFullGroupCircle(),
        m_pDocument->show80PercentCircle(),
        m_pDocument->show90PercentCircle()
    );
}

QString MarkImpactsState::stateName() const
{
    return tr("Mark Impacts");
}

double MarkImpactsState::bulletDiameterPixels() const
{
    if (!m_pDocument || !m_pDocument->hasScaleFactorSet())
    {
        return kDefaultBulletDiameterPixels;
    }
    return m_pDocument->bulletDiameter() * m_pDocument->pixelsPerInch();
}
