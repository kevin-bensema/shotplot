#include "WorkflowToolbar.h"
#include "Core/ShotGroupDocument.h"

#include <QToolButton>
#include <QLabel>

const QStringList WorkflowToolbar::STATE_NAMES = {
    "Caliber",
    "Scale",
    "POA",
    "Mark Impacts",
    "Visualization"
};

WorkflowToolbar::WorkflowToolbar(QWidget *parent)
    : QToolBar(parent)
{
    setWindowTitle(tr("Workflow"));
    setMovable(false);
    setupButtons();
}

WorkflowToolbar::~WorkflowToolbar() = default;

void WorkflowToolbar::setupButtons()
{
    for (int i = 0; i < STATE_NAMES.size(); ++i)
    {
        if (i > 0)
        {
            QLabel *arrow = new QLabel(" → ");
            addWidget(arrow);
        }
        
        QToolButton *button = new QToolButton(this);
        button->setText(STATE_NAMES[i]);
        button->setCheckable(true);
        button->setEnabled(false);
        button->setProperty("stateIndex", i);
        
        connect(button, &QToolButton::clicked, this, &WorkflowToolbar::onButtonClicked);
        
        addWidget(button);
        m_buttons.append(button);
    }
}

void WorkflowToolbar::setDocument(ShotGroupDocument *doc)
{
    m_document = doc;
    
    if (m_document)
    {
        connect(m_document, &ShotGroupDocument::dataChanged,
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
    if (!m_document)
    {
        for (auto *button : m_buttons)
        {
            button->setEnabled(false);
        }
        return;
    }
    
    // State 0: Caliber - always enabled when document exists
    m_buttons[0]->setEnabled(true);
    
    // State 1: Scale - enabled after caliber is set
    m_buttons[1]->setEnabled(m_document->canEnableScaleFactorState());
    
    // State 2: POA - enabled after scale factor is set
    m_buttons[2]->setEnabled(m_document->canEnablePointOfAimState());
    
    // State 3: Mark Impacts - enabled after scale factor is set
    m_buttons[3]->setEnabled(m_document->canEnableMarkImpactsState());
    
    // State 4: Visualization - enabled after at least one impact
    m_buttons[4]->setEnabled(m_document->canEnableVisualizationState());
}

void WorkflowToolbar::onButtonClicked()
{
    QToolButton *button = qobject_cast<QToolButton*>(sender());
    if (!button) return;
    
    int index = button->property("stateIndex").toInt();
    emit stateSelected(index);
}
