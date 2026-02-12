#include "MarkImpactsState.h"
#include <Core/ShotGroupDocument.h>
#include <Core/ShotImpact.h>
#include <Widgets/TargetView.h>
#include <Graphics/TargetScene.h>
#include <Commands/AddImpactCommand.h>
#include <Commands/ClearImpactsCommand.h>
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QUndoStack>

namespace {
    constexpr double kDefaultBulletDiameterPixels = 20.0;
}

MarkImpactsState::MarkImpactsState(ShotGroupDocument* pDocument, TargetView* pView, 
                                   QUndoStack* pUndoStack)
    : WorkflowState(pDocument, pView)
    , m_pView(pView)
    , m_pUndoStack(pUndoStack)
{
}

MarkImpactsState::~MarkImpactsState() = default;

void MarkImpactsState::onEnter()
{
    // Connect to document changes for updating circles
    // Use UniqueConnection to avoid multiple connections if setDocument was called while active
    connect(m_pDocument, &ShotGroupDocument::impactsChanged,
            this, &MarkImpactsState::updateGroupCircles, Qt::UniqueConnection);
    
    updateGroupCircles();
}

void MarkImpactsState::onExit()
{
    disconnect(m_pDocument, &ShotGroupDocument::impactsChanged,
               this, &MarkImpactsState::updateGroupCircles);
}

void MarkImpactsState::handleMouseClick(const QPointF& scenePos)
{
    // Create new impact
    int id = m_pDocument->nextImpactId();
    ShotImpact impact(id, scenePos.x(), scenePos.y());
    
    // Use undo command
    m_pUndoStack->push(new AddImpactCommand(m_pDocument, m_pView->targetScene(), impact, bulletDiameterPixels()));
}

void MarkImpactsState::handleRightClick(const QPointF& scenePos)
{
    // Find and remove impact at this position
    // For now, simple distance-based hit test
    double hitRadius = bulletDiameterPixels() / 2.0;
    
    const auto impacts = m_pDocument->impacts();
    for (int i = 0; i < impacts.size(); ++i) {
        QPointF impactPos = impacts[i].position();
        double dx = scenePos.x() - impactPos.x();
        double dy = scenePos.y() - impactPos.y();
        double dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist <= hitRadius) {
            // Remove this impact
            // TODO: Use RemoveImpactCommand for undo support
            m_pView->targetScene()->removeImpactGlyph(impacts[i].id);
            m_pDocument->removeImpact(i);
            break;
        }
    }
}

bool MarkImpactsState::isComplete() const
{
    return m_pDocument && m_pDocument->impactCount() >= 1;
}

QCursor MarkImpactsState::cursor() const
{
    return QCursor(Qt::CrossCursor);
}

void MarkImpactsState::populateToolbar(QToolBar* pToolbar)
{
    pToolbar->addWidget(new QLabel(tr("Mark all impacts")));
    
    pToolbar->addSeparator();
    
    QPushButton* pDoneButton = new QPushButton(tr("Done marking impacts"));
    pDoneButton->setDefault(true);
    connect(pDoneButton, &QPushButton::clicked, this, [this]() {
        emit requestNextState();
    });
    pToolbar->addWidget(pDoneButton);

    pToolbar->addSeparator();

    QPushButton* pClearButton = new QPushButton(tr("Clear All"));
    connect(pClearButton, &QPushButton::clicked, [this]() {
        if (m_pDocument->impactCount() > 0)
        {
            m_pUndoStack->push(new ClearImpactsCommand(m_pDocument, m_pView->targetScene()));
        }
    });
    pToolbar->addWidget(pClearButton);
}

void MarkImpactsState::updateGroupCircles()
{
    auto* pScene = m_pView->targetScene();
    
    if (m_pDocument->impactCount() < 2)
    {
        pScene->clearGroupCircles();
        return;
    }
    
    const Statistics& stats = m_pDocument->statistics();
    
    if (stats.fullGroupCircle.isValid())
    {
        pScene->setGroupCircle(GroupCircle::Type::Full, stats.fullGroupCircle.center, stats.fullGroupCircle.radiusPixels);
    }
    
    if (stats.group80Circle.isValid())
    {
        pScene->setGroupCircle(GroupCircle::Type::Percent80, stats.group80Circle.center, stats.group80Circle.radiusPixels);
    }
    
    if (stats.group90Circle.isValid())
    {
        pScene->setGroupCircle(GroupCircle::Type::Percent90, stats.group90Circle.center, stats.group90Circle.radiusPixels);
    }
    
    // Update visibility based on document settings
    pScene->updateGroupCirclesVisibility();
}

QString MarkImpactsState::stateName() const
{
    return tr("Mark Impacts");
}

void MarkImpactsState::setDocument(ShotGroupDocument* pDocument)
{
    if (m_pDocument)
    {
        disconnect(m_pDocument, &ShotGroupDocument::impactsChanged,
                   this, &MarkImpactsState::updateGroupCircles);
    }

    WorkflowState::setDocument(pDocument);
}

void MarkImpactsState::drawCursor(QPainter* pPainter, const QPointF& viewportPos, double zoomFactor)
{
    if (!m_pDocument || !m_pDocument->hasScaleFactorSet()) return;

    // Calculate diameter in pixels
    double diameterInch = m_pDocument->bulletDiameter();
    double ppi = m_pDocument->pixelsPerInch();
    double diameterPixels = diameterInch * ppi * zoomFactor;

    // Get color from settings
    auto& graphicsService = qx::GetService<GraphicsSettingsService>();
    QColor color = graphicsService.color(GraphicsSettingsService::ColorRole::ImpactCursor);

    // Draw segmented circle
    double radius = diameterPixels / 2.0;
    constexpr double kLineWidth = 2.0;
    constexpr double kGapAngle = 15.0;

    QPen pen(color);
    pen.setWidthF(kLineWidth);
    pen.setCapStyle(Qt::FlatCap);
    pPainter->setPen(pen);
    pPainter->setBrush(Qt::NoBrush);

    QRectF rect(viewportPos.x() - radius, viewportPos.y() - radius, 
                diameterPixels, diameterPixels);

    double arcSpan = 90.0 - kGapAngle;
    double halfGap = kGapAngle / 2.0;

    // Draw the 4 arcs
    pPainter->drawArc(rect, (90 + halfGap) * 16, arcSpan * 16);
    pPainter->drawArc(rect, (180 + halfGap) * 16, arcSpan * 16);
    pPainter->drawArc(rect, (270 + halfGap) * 16, arcSpan * 16);
    pPainter->drawArc(rect, (0 + halfGap) * 16, arcSpan * 16);
}

double MarkImpactsState::bulletDiameterPixels() const
{
    if (!m_pDocument || !m_pDocument->hasScaleFactorSet())
    {
        return kDefaultBulletDiameterPixels;
    }
    return m_pDocument->bulletDiameter() * m_pDocument->pixelsPerInch();
}
