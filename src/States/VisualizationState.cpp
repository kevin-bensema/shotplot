#include "VisualizationState.h"
#include <Core/ShotGroupDocument.h>
#include <Widgets/TargetView.h>
#include <Graphics/TargetScene.h>

#include <QToolBar>
#include <QLabel>

VisualizationState::VisualizationState(ShotGroupDocument* pDocument, TargetView* pView, TargetScene* pScene)
    : WorkflowState(pDocument, pView)
    , m_pView(pView)
    , m_pScene(pScene)
{
}

VisualizationState::~VisualizationState() = default;

void VisualizationState::onEnter()
{
    // TODO: Set up visualization configuration UI
}

void VisualizationState::onExit()
{
    // TODO: Clean up
}

void VisualizationState::handleMouseClick(const QPointF& scenePos)
{
    Q_UNUSED(scenePos)
    // TODO: Handle plaque placement/dragging
}

bool VisualizationState::isComplete() const
{
    return m_pDocument && m_pDocument->impactCount() >= 1;
}

QString VisualizationState::stateName() const
{
    return tr("Visualization");
}

void VisualizationState::populateToolbar(QToolBar* pToolbar)
{
    pToolbar->addWidget(new QLabel(tr("Configure visualization (not yet implemented)")));
}
