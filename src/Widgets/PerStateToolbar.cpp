#include "PerStateToolbar.h"
#include <States/WorkflowState.h>

PerStateToolbar::PerStateToolbar(QWidget* pParent)
    : QToolBar(pParent)
{
    setWindowTitle(tr("State Controls"));
    setMovable(false);
}

PerStateToolbar::~PerStateToolbar() = default;

void PerStateToolbar::setCurrentState(WorkflowState* pState)
{
    clearToolbar();
    m_pCurrentState = pState;
    
    if (m_pCurrentState)
    {
        m_pCurrentState->populateToolbar(this);
    }
}

void PerStateToolbar::clearToolbar()
{
    clear();
}
