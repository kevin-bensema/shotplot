#include "POAState.h"
#include <Core/ShotGroupDocument.h>
#include <Widgets/TargetView.h>
#include <Graphics/TargetScene.h>

#include <QToolBar>
#include <QLabel>
#include <QPushButton>

POAState::POAState(ShotGroupDocument* pDocument, TargetView* pView)
    : WorkflowState(pDocument, pView)
    , m_pView(pView)
{
}

POAState::~POAState() = default;

void POAState::onEnter()
{
    // Show existing POA if any
    if (m_pDocument->hasPointOfAimSet())
    {
        m_pView->targetScene()->setPOAGlyph(m_pDocument->pointOfAim());
    }
}

void POAState::onExit()
{
    // POA glyph stays visible
}

void POAState::handleMouseClick(const QPointF& scenePos)
{
    m_pDocument->setPointOfAim(scenePos);
    m_pView->targetScene()->setPOAGlyph(scenePos);
    
    emit stateCompleted();
    emit requestNextState();
}

bool POAState::isComplete() const
{
    // POA is optional, so we consider it complete if scale factor is set
    return m_pDocument && m_pDocument->hasScaleFactorSet();
}

QCursor POAState::cursor() const
{
    // X-shaped cursor (we'll use a built-in for now)
    return QCursor(Qt::CrossCursor);
}

QString POAState::stateName() const
{
    return tr("Point of Aim");
}

void POAState::populateToolbar(QToolBar* pToolbar)
{
    pToolbar->addWidget(new QLabel(tr("Click to mark your intended point of impact")));
    
    // Skip button
    QPushButton* pSkipButton = new QPushButton(tr("Skip"));
    connect(pSkipButton, &QPushButton::clicked, this, [this]()
    {
        emit requestNextState();
    });
    pToolbar->addWidget(pSkipButton);
}
