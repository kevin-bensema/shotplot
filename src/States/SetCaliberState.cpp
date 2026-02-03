#include "SetCaliberState.h"
#include "Core/ShotGroupDocument.h"
#include "Widgets/TargetView.h"
#include "Widgets/CaliberDialog.h"

#include <QToolBar>
#include <QLabel>

SetCaliberState::SetCaliberState(ShotGroupDocument *document, TargetView *view, QWidget *parentWidget)
    : WorkflowState(document, view)
    , m_view(view)
    , m_parentWidget(parentWidget)
{
}

SetCaliberState::~SetCaliberState() = default;

void SetCaliberState::onEnter()
{
    // Show caliber dialog
    CaliberDialog dialog(m_parentWidget);
    
    if (m_document->hasCaliberSet())
    {
        dialog.setBulletDiameter(m_document->bulletDiameter());
    }
    
    if (dialog.exec() == QDialog::Accepted)
    {
        m_document->setBulletDiameter(dialog.bulletDiameter());
        emit stateCompleted();
        emit requestNextState();
    }
}

void SetCaliberState::onExit()
{
    // Nothing to clean up
}

void SetCaliberState::handleMouseClick(const QPointF &scenePos)
{
    Q_UNUSED(scenePos)
    // This state uses a dialog, not mouse clicks
}

bool SetCaliberState::isComplete() const
{
    return m_document && m_document->hasCaliberSet();
}

void SetCaliberState::populateToolbar(QToolBar *toolbar)
{
    toolbar->addWidget(new QLabel(tr("Select bullet diameter from the dialog")));
}
