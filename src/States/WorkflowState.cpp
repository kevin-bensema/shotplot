#include "WorkflowState.h"

WorkflowState::WorkflowState(ShotGroupDocument* pDocument, QObject* pParent)
    : QObject(pParent)
    , m_pDocument(pDocument)
{
}

WorkflowState::~WorkflowState() = default;

void WorkflowState::handleRightClick(const QPointF& scenePos)
{
    Q_UNUSED(scenePos)
    // Default: do nothing
}

void WorkflowState::handleMouseMove(const QPointF& scenePos)
{
    Q_UNUSED(scenePos)
    // Default: do nothing
}

QCursor WorkflowState::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}
