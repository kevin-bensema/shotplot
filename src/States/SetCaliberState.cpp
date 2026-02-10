#include "SetCaliberState.h"
#include <Core/ShotGroupDocument.h>
#include <Widgets/TargetView.h>
#include <Widgets/CaliberDialog.h>

#include <QToolBar>
#include <QLabel>

SetCaliberState::SetCaliberState(ShotGroupDocument* pDocument, TargetView* pView, QWidget* pParentWidget)
    : WorkflowState(pDocument, pView)
    , m_pView(pView)
    , m_pParentWidget(pParentWidget)
{
}

SetCaliberState::~SetCaliberState() = default;

void SetCaliberState::onEnter()
{
    bool alreadyHasCaliber = m_pDocument->hasCaliberSet();

    // Show caliber dialog
    CaliberDialog dialog(m_pParentWidget);
    
    if (alreadyHasCaliber)
    {
        dialog.setBulletDiameter(m_pDocument->bulletDiameter());
    }
    
    if (dialog.exec() == QDialog::Accepted)
    {
        m_pDocument->setBulletDiameter(dialog.bulletDiameter());
        emit stateCompleted();
        
        if (!alreadyHasCaliber)
        {
            emit requestNextState();
        }
    }
}

void SetCaliberState::onExit()
{
    // Nothing to clean up
}

void SetCaliberState::handleMouseClick(const QPointF& scenePos)
{
    Q_UNUSED(scenePos)
    // This state uses a dialog, not mouse clicks
}

bool SetCaliberState::isComplete() const
{
    return m_pDocument && m_pDocument->hasCaliberSet();
}

void SetCaliberState::populateToolbar(QToolBar* pToolbar)
{
    pToolbar->addWidget(new QLabel(tr("Select bullet diameter from the dialog")));
}

QString SetCaliberState::stateName() const
{
    return tr("Set Caliber");
}
