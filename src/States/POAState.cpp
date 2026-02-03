#include "POAState.h"
#include "Core/ShotGroupDocument.h"
#include "Widgets/TargetView.h"
#include "Graphics/TargetScene.h"

#include <QToolBar>
#include <QLabel>
#include <QPushButton>

POAState::POAState(ShotGroupDocument *document, TargetView *view, TargetScene *scene)
    : WorkflowState(document, view)
    , m_view(view)
    , m_scene(scene)
{
}

POAState::~POAState() = default;

void POAState::onEnter()
{
    // Show existing POA if any
    if (m_document->hasPointOfAimSet())
    {
        m_scene->setPOAGlyph(m_document->pointOfAim());
    }
}

void POAState::onExit()
{
    // POA glyph stays visible
}

void POAState::handleMouseClick(const QPointF &scenePos)
{
    m_document->setPointOfAim(scenePos);
    m_scene->setPOAGlyph(scenePos);
    
    emit stateCompleted();
    emit requestNextState();
}

bool POAState::isComplete() const
{
    // POA is optional, so we consider it complete if scale factor is set
    return m_document && m_document->hasScaleFactorSet();
}

QCursor POAState::cursor() const
{
    // X-shaped cursor (we'll use a built-in for now)
    return QCursor(Qt::CrossCursor);
}

void POAState::populateToolbar(QToolBar *toolbar)
{
    toolbar->addWidget(new QLabel(tr("Click to mark your intended point of impact")));
    
    // Skip button
    QPushButton *skipButton = new QPushButton(tr("Skip"));
    connect(skipButton, &QPushButton::clicked, this, [this]()
    {
        emit requestNextState();
    });
    toolbar->addWidget(skipButton);
}
