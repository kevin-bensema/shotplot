#include "PerStateToolbar.h"
#include <States/WorkflowState.h>
#include <QWidget>

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
        auto makeSpacer = [this]() {
            QWidget* pSpacer = new QWidget(this);
            pSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            return pSpacer;
        };

        addWidget(makeSpacer());
        m_pCurrentState->populateToolbar(this);
        addWidget(makeSpacer());
    }
}

void PerStateToolbar::clearToolbar()
{
    clear();
}
