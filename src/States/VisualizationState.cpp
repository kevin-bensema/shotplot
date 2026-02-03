#include "VisualizationState.h"
#include "Core/ShotGroupDocument.h"
#include "Widgets/TargetView.h"
#include "Graphics/TargetScene.h"

#include <QToolBar>
#include <QLabel>

VisualizationState::VisualizationState(ShotGroupDocument *document, TargetView *view, TargetScene *scene)
    : WorkflowState(document, view)
    , m_view(view)
    , m_scene(scene)
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

void VisualizationState::handleMouseClick(const QPointF &scenePos)
{
    Q_UNUSED(scenePos)
    // TODO: Handle plaque placement/dragging
}

bool VisualizationState::isComplete() const
{
    return m_document && m_document->impactCount() >= 1;
}

void VisualizationState::populateToolbar(QToolBar *toolbar)
{
    toolbar->addWidget(new QLabel(tr("Configure visualization (not yet implemented)")));
}
