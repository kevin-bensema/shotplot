#include "WorkflowToolbar.h"
#include "Core/ShotGroupDocument.h"
#include "States/WorkflowState.h"

#include <QToolButton>
#include <QLabel>

namespace {
    const QStringList kStateNames = {
        "Caliber",
        "Scale",
        "POA",
        "Mark Impacts",
        "Visualization"
    };
}

WorkflowToolbar::WorkflowToolbar(QWidget* pParent)
    : QToolBar(pParent)
{
    setWindowTitle(tr("Workflow"));
    setMovable(false);
    setupButtons();
}

WorkflowToolbar::~WorkflowToolbar() = default;

void WorkflowToolbar::setupButtons()
{
    auto makeSpacer = [this]() {
        QWidget* pSpacer = new QWidget(this);
        pSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        return pSpacer;
    };

    addWidget(makeSpacer());

    for (int i = 0; i < kStateNames.size(); ++i)
    {
        if (i > 0)
        {
            QLabel* pArrow = new QLabel(" → ");
            addWidget(pArrow);
        }
        
        QToolButton* pButton = new QToolButton(this);
        pButton->setText(kStateNames[i]);
        pButton->setCheckable(true);
        pButton->setEnabled(false);
        pButton->setProperty("stateIndex", i);
        
        connect(pButton, &QToolButton::clicked, this, [this]() {
            QToolButton* pSenderButton = qobject_cast<QToolButton*>(sender());
            if (!pSenderButton) return;
            
            int index = pSenderButton->property("stateIndex").toInt();
            emit stateSelected(index);
        });
        
        addWidget(pButton);
        m_buttons.append(pButton);
    }

    addWidget(makeSpacer());
}

void WorkflowToolbar::setDocument(ShotGroupDocument* pDocument)
{
    if (m_pDocument)
    {
        m_pDocument->disconnect(this);
    }

    m_pDocument = pDocument;
    
    if (m_pDocument)
    {
        connect(m_pDocument, &ShotGroupDocument::dataChanged,
                this, &WorkflowToolbar::updateStateEnablement);
    }
    
    updateStateEnablement();
}

void WorkflowToolbar::setCurrentState(int index)
{
    m_currentState = index;
    
    for (int i = 0; i < m_buttons.size(); ++i)
    {
        m_buttons[i]->setChecked(i == index);
    }
}

void WorkflowToolbar::updateStateEnablement()
{
    if (!m_pDocument)
    {
        for (auto* pButton : m_buttons)
        {
            pButton->setEnabled(false);
        }
        return;
    }
    
    // State 0: Caliber - always enabled when document exists
    m_buttons[WorkflowState::kSetCaliberStateIndex]->setEnabled(true);
    
    // State 1: Scale - enabled after caliber is set
    m_buttons[WorkflowState::kScaleFactorStateIndex]->setEnabled(m_pDocument->canEnableScaleFactorState());
    
    // State 2: POA - enabled after scale factor is set
    m_buttons[WorkflowState::kPOAStateIndex]->setEnabled(m_pDocument->canEnablePointOfAimState());
    
    // State 3: Mark Impacts - enabled after scale factor is set
    m_buttons[WorkflowState::kMarkImpactsStateIndex]->setEnabled(m_pDocument->canEnableMarkImpactsState());
    
    // State 4: Visualization - enabled after at least one impact
    m_buttons[WorkflowState::kVisualizationStateIndex]->setEnabled(m_pDocument->canEnableVisualizationState());
}
