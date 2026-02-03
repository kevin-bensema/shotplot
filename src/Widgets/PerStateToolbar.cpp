#include "PerStateToolbar.h"
#include "States/WorkflowState.h"

PerStateToolbar::PerStateToolbar(QWidget *parent)
    : QToolBar(parent)
{
    setWindowTitle(tr("State Controls"));
    setMovable(false);
}

PerStateToolbar::~PerStateToolbar() = default;

void PerStateToolbar::setCurrentState(WorkflowState *state)
{
    clearToolbar();
    m_currentState = state;
    
    if (m_currentState)
    {
        m_currentState->populateToolbar(this);
    }
}

void PerStateToolbar::clearToolbar()
{
    clear();
}
