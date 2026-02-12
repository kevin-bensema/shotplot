#include "ScaleFactorState.h"
#include <Core/ShotGroupDocument.h>
#include <Widgets/TargetView.h>
#include <Graphics/TargetScene.h>

#include <QToolBar>
#include <QLabel>
#include <QDoubleSpinBox>
#include <cmath>

namespace {
    constexpr double kMinDistanceInches = 0.1;
    constexpr double kMaxDistanceInches = 100.0;
    constexpr double kDefaultDistanceInches = 1.0;
    constexpr int kDistanceDecimals = 2;
    constexpr double kDistanceSingleStep = 0.1;
}

ScaleFactorState::ScaleFactorState(ShotGroupDocument* pDocument, TargetView* pView)
    : WorkflowState(pDocument, pView)
    , m_pView(pView)
{
}

ScaleFactorState::~ScaleFactorState() = default;

void ScaleFactorState::onEnter()
{
    reset();
}

void ScaleFactorState::onExit()
{
    m_pView->targetScene()->hideScaleLine();
    m_hasFirstPoint = false;
}

void ScaleFactorState::handleMouseClick(const QPointF& scenePos)
{
    if (!m_hasFirstPoint)
    {
        // First click - set start point
        m_firstPoint = scenePos;
        m_hasFirstPoint = true;
        m_pView->targetScene()->setScaleLineStart(scenePos);
    }
    else
    {
        // Second click - calculate scale factor
        m_pView->targetScene()->showScaleLine(m_firstPoint, scenePos);
        
        // Calculate distance in pixels
        double dx = scenePos.x() - m_firstPoint.x();
        double dy = scenePos.y() - m_firstPoint.y();
        double distancePixels = std::sqrt(dx * dx + dy * dy);
        
        // Get reference distance from toolbar spinbox
        double distanceInches = m_pDistanceSpin ? m_pDistanceSpin->value() : kDefaultDistanceInches;
        
        if (distancePixels > 0 && distanceInches > 0)
        {
            double pixelsPerInch = distancePixels / distanceInches;
            m_pDocument->setPixelsPerInch(pixelsPerInch);
        }
        
        // Reset for potential recalibration
        m_hasFirstPoint = false;
        m_pView->targetScene()->hideScaleLine();
        
        emit stateCompleted();
        emit requestNextState();
    }
}

void ScaleFactorState::handleMouseMove(const QPointF& scenePos)
{
    if (m_hasFirstPoint)
    {
        m_pView->targetScene()->updateScaleLineEnd(scenePos);
    }
}

bool ScaleFactorState::isComplete() const
{
    return m_pDocument && m_pDocument->hasScaleFactorSet();
}

QCursor ScaleFactorState::cursor() const
{
    return QCursor(Qt::CrossCursor);
}

void ScaleFactorState::populateToolbar(QToolBar* pToolbar)
{
    pToolbar->addWidget(new QLabel(tr("Select two points")));
    
    m_pDistanceSpin = new QDoubleSpinBox();
    m_pDistanceSpin->setRange(kMinDistanceInches, kMaxDistanceInches);
    m_pDistanceSpin->setValue(kDefaultDistanceInches);
    m_pDistanceSpin->setDecimals(kDistanceDecimals);
    m_pDistanceSpin->setSingleStep(kDistanceSingleStep);
    pToolbar->addWidget(m_pDistanceSpin);
    
    pToolbar->addWidget(new QLabel(tr("inches apart")));
}

void ScaleFactorState::reset()
{
    m_hasFirstPoint = false;
    m_pView->targetScene()->hideScaleLine();
}

QString ScaleFactorState::stateName() const
{
    return tr("Scale Factor");
}

void ScaleFactorState::setDocument(ShotGroupDocument* pDocument)
{
    WorkflowState::setDocument(pDocument);
    reset();
}
