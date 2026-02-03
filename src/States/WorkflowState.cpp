#include "WorkflowState.h"

WorkflowState::WorkflowState(ShotGroupDocument *document, QObject *parent)
    : QObject(parent)
    , m_document(document)
{
}

WorkflowState::~WorkflowState() = default;

void WorkflowState::handleRightClick(const QPointF &scenePos)
{
    Q_UNUSED(scenePos)
    // Default: do nothing
}

void WorkflowState::handleMouseMove(const QPointF &scenePos)
{
    Q_UNUSED(scenePos)
    // Default: do nothing
}

QCursor WorkflowState::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}
