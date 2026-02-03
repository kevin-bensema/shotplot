#include "ScaleFactorState.h"
#include "Core/ShotGroupDocument.h"
#include "Widgets/TargetView.h"
#include "Graphics/TargetScene.h"

#include <QToolBar>
#include <QLabel>
#include <QDoubleSpinBox>
#include <cmath>

ScaleFactorState::ScaleFactorState(ShotGroupDocument *document, TargetView *view, TargetScene *scene)
    : WorkflowState(document, view)
    , m_view(view)
    , m_scene(scene)
{
}

ScaleFactorState::~ScaleFactorState() = default;

void ScaleFactorState::onEnter()
{
    reset();
}

void ScaleFactorState::onExit()
{
    m_scene->hideScaleLine();
    m_hasFirstPoint = false;
}

void ScaleFactorState::handleMouseClick(const QPointF &scenePos)
{
    if (!m_hasFirstPoint)
    {
        // First click - set start point
        m_firstPoint = scenePos;
        m_hasFirstPoint = true;
        m_scene->setScaleLineStart(scenePos);
    }
    else
    {
        // Second click - calculate scale factor
        m_scene->showScaleLine(m_firstPoint, scenePos);
        
        // Calculate distance in pixels
        double dx = scenePos.x() - m_firstPoint.x();
        double dy = scenePos.y() - m_firstPoint.y();
        double distancePixels = std::sqrt(dx * dx + dy * dy);
        
        // Get reference distance from toolbar spinbox
        double distanceInches = m_distanceSpin ? m_distanceSpin->value() : 1.0;
        
        if (distancePixels > 0 && distanceInches > 0)
        {
            double pixelsPerInch = distancePixels / distanceInches;
            m_document->setPixelsPerInch(pixelsPerInch);
        }
        
        // Reset for potential recalibration
        m_hasFirstPoint = false;
        m_scene->hideScaleLine();
        
        emit stateCompleted();
        emit requestNextState();
    }
}

void ScaleFactorState::handleMouseMove(const QPointF &scenePos)
{
    if (m_hasFirstPoint)
    {
        m_scene->updateScaleLineEnd(scenePos);
    }
}

bool ScaleFactorState::isComplete() const
{
    return m_document && m_document->hasScaleFactorSet();
}

QCursor ScaleFactorState::cursor() const
{
    return QCursor(Qt::CrossCursor);
}

void ScaleFactorState::populateToolbar(QToolBar *toolbar)
{
    toolbar->addWidget(new QLabel(tr("Select two points")));
    
    m_distanceSpin = new QDoubleSpinBox();
    m_distanceSpin->setRange(0.1, 100.0);
    m_distanceSpin->setValue(1.0);
    m_distanceSpin->setDecimals(2);
    m_distanceSpin->setSingleStep(0.1);
    toolbar->addWidget(m_distanceSpin);
    
    toolbar->addWidget(new QLabel(tr("inches apart")));
}

void ScaleFactorState::reset()
{
    m_hasFirstPoint = false;
    m_scene->hideScaleLine();
}
