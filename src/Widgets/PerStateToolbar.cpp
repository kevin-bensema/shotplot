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
    m_pCurrentState = state;
    
    if (m_pCurrentState)
    {
        m_pCurrentState->populateToolbar(this);
    }
}

void PerStateToolbar::clearToolbar()
{
    clear();
}
