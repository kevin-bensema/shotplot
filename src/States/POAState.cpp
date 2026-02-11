#include "POAState.h"
#include <Core/ShotGroupDocument.h>
#include <Widgets/TargetView.h>
#include <Graphics/TargetScene.h>
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QPainter>

POAState::POAState(ShotGroupDocument* pDocument, TargetView* pView)
    : WorkflowState(pDocument, pView)
    , m_pView(pView)
{
}

POAState::~POAState() = default;

void POAState::onEnter()
{
}

void POAState::onExit()
{
    // POA glyph stays visible
}

void POAState::handleMouseClick(const QPointF& scenePos)
{
    m_pDocument->setPointOfAim(scenePos);
    
    // Calculate POA glyph diameter: 1.2x bullet diameter
    double diameter = m_pDocument->bulletDiameter() * m_pDocument->pixelsPerInch();
    double poaDiameter = diameter * 1.2;
    
    m_pView->targetScene()->setPOAGlyph(scenePos, poaDiameter);
    
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
    return QCursor(Qt::CrossCursor);
}

QString POAState::stateName() const
{
    return tr("Point of Aim");
}

void POAState::drawCursor(QPainter* pPainter, const QPointF& viewportPos, double zoomFactor)
{
    if (!m_pDocument || !m_pDocument->hasScaleFactorSet()) return;

    // Calculate diameter in pixels
    double diameterInch = m_pDocument->bulletDiameter();
    double ppi = m_pDocument->pixelsPerInch();
    double diameterPixels = diameterInch * ppi * zoomFactor;

    // Get color from settings
    auto& graphicsService = qx::GetService<GraphicsSettingsService>();
    QColor color = graphicsService.color(GraphicsSettingsService::ColorRole::PointOfAimCursor);

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
