#include "TargetView.h"
#include <Graphics/TargetScene.h>
#include <States/WorkflowState.h>
#include <Services/GraphicsSettingsService.h>
#include <QX/Services.h>

#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPainter>
#include <QCursor>
#include <cmath>

namespace {
    constexpr double kZoomStep = 1.15;
    constexpr double kMinZoom = 0.1;
    constexpr double kMaxZoom = 10.0;
    constexpr double kClickThreshold = 5.0;  // pixels
}

TargetView::TargetView(QWidget* pParent)
    : QGraphicsView(pParent)
{
    // Disable scroll bars - pan/zoom handled manually
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Smooth rendering
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Viewport settings
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    
    // Enable mouse tracking for cursor updates
    setMouseTracking(true);
    
    // Background - get from graphics settings service
    updateBackgroundColor();
    
    // Drag mode - we handle this manually
    setDragMode(QGraphicsView::NoDrag);
    
    // Connect to graphics settings service for color/opacity changes
    auto& graphicsService = qx::GetService<GraphicsSettingsService>();
    connect(&graphicsService, &GraphicsSettingsService::settingsChanged, this, [this]() {
        updateBackgroundColor();
        viewport()->update();
    });
}

void TargetView::setTargetScene(TargetScene* pScene)
{
    m_pTargetScene = pScene;
    setScene(pScene);
}

TargetScene* TargetView::targetScene() const
{
    return m_pTargetScene;
}

void TargetView::setWorkflowState(WorkflowState* pState)
{
    m_pCurrentState = pState;
    
    // Update current mouse position to avoid drawing at (0,0) initially
    m_currentMousePos = mapFromGlobal(QCursor::pos());
    
    updateCursor();
    viewport()->update(); // Trigger viewport repaint to show custom cursor immediately
}

WorkflowState* TargetView::workflowState() const
{
    return m_pCurrentState;
}

void TargetView::zoomIn()
{
    double newZoom = transform().m11() * kZoomStep;
    if (newZoom <= kMaxZoom)
    {
        scale(kZoomStep, kZoomStep);
    }
}

void TargetView::zoomOut()
{
    double newZoom = transform().m11() / kZoomStep;
    if (newZoom >= kMinZoom)
    {
        scale(1.0 / kZoomStep, 1.0 / kZoomStep);
    }
}

void TargetView::zoomFit()
{
    if (!m_pTargetScene) return;
    
    // Reset transform
    resetTransform();
    
    // Fit scene in view
    QRectF sceneRect = m_pTargetScene->sceneRect();
    if (!sceneRect.isEmpty())
    {
        // Use the typed accessor to get the target image rect directly
        QRectF imageRect;
        if (auto* pImageItem = m_pTargetScene->targetImageItem())
        {
            imageRect = pImageItem->sceneBoundingRect();
        }

        if (!imageRect.isEmpty())
        {
            fitInView(imageRect, Qt::KeepAspectRatio);
        }
        else
        {
            fitInView(sceneRect, Qt::KeepAspectRatio);
        }
    }
}

void TargetView::wheelEvent(QWheelEvent* pEvent)
{
    // Zoom with mouse wheel
    if (pEvent->angleDelta().y() > 0)
    {
        zoomIn();
    }
    else
    {
        zoomOut();
    }
    pEvent->accept();
}

void TargetView::mousePressEvent(QMouseEvent* pEvent)
{
    m_mousePressPos = pEvent->pos();
    m_lastMousePos = pEvent->pos();
    
    // Middle button or Ctrl+left always pans
    if (pEvent->button() == Qt::MiddleButton ||
        (pEvent->button() == Qt::LeftButton && (pEvent->modifiers() & Qt::ControlModifier)))
    {
        m_isPanning = true;
        setCursor(Qt::ClosedHandCursor);
        pEvent->accept();
        return;
    }
    
    // Right button - context menu or state action
    if (pEvent->button() == Qt::RightButton && m_pCurrentState)
    {
        QPointF scenePos = mapToScene(pEvent->pos());
        m_pCurrentState->handleRightClick(scenePos);
        pEvent->accept();
        return;
    }
    
    // Left button - forward to scene first so items can grab the mouse,
    // then handle click-vs-drag on release if no item grabbed it
    QGraphicsView::mousePressEvent(pEvent);
}

bool TargetView::isItemGrabbingMouse() const
{
    return scene() && scene()->mouseGrabberItem();
}

void TargetView::mouseMoveEvent(QMouseEvent* pEvent)
{
    m_currentMousePos = pEvent->pos();

    // Emit position for status bar
    QPointF scenePos = mapToScene(pEvent->pos());
    emit mousePositionChanged(scenePos);
    
    if (m_isPanning)
    {
        // Pan the view
        QPointF delta = pEvent->pos() - m_lastMousePos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_lastMousePos = pEvent->pos();
        pEvent->accept();
        viewport()->update(); // Trigger viewport repaint for custom cursor
        return;
    }
    
    // Check if we've dragged far enough to start panning, but only if
    // no scene item has grabbed the mouse (e.g. plaque drag/resize)
    if ((pEvent->buttons() & Qt::LeftButton) && !isItemGrabbingMouse())
    {
        QPointF delta = pEvent->pos() - m_mousePressPos;
        double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
        
        if (distance >= kClickThreshold && !m_isPanning)
        {
            // Start panning
            m_isPanning = true;
            setCursor(Qt::ClosedHandCursor);
        }
    }
    
    // Delegate to state for hover effects, line drawing, etc.
    if (m_pCurrentState)
    {
        m_pCurrentState->handleMouseMove(scenePos);
    }
    
    QGraphicsView::mouseMoveEvent(pEvent);
    viewport()->update(); // Trigger viewport repaint for custom cursor
}

void TargetView::mouseReleaseEvent(QMouseEvent* pEvent)
{
    if (m_isPanning)
    {
        m_isPanning = false;
        updateCursor();
        pEvent->accept();
        return;
    }
    
    // If a scene item was handling the mouse (e.g. plaque drag/resize),
    // just let the base class finalize it — don't interpret as a view click
    if (isItemGrabbingMouse())
    {
        QGraphicsView::mouseReleaseEvent(pEvent);
        return;
    }

    // Check for click vs drag
    QPointF delta = pEvent->pos() - m_mousePressPos;
    double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
    
    if (distance < kClickThreshold && pEvent->button() == Qt::LeftButton)
    {
        // This was a click - delegate to state
        if (m_pCurrentState)
        {
            QPointF scenePos = mapToScene(pEvent->pos());
            m_pCurrentState->handleMouseClick(scenePos);
        }
    }
    
    QGraphicsView::mouseReleaseEvent(pEvent);
}

void TargetView::updateCursor()
{
    if (m_pCurrentState)
    {
        setCursor(m_pCurrentState->cursor());
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void TargetView::updateBackgroundColor()
{
    auto const& graphicsService = qx::GetService<GraphicsSettingsService>();
    const QColor backgroundColor = graphicsService.color(GraphicsSettingsService::ColorRole::Background);
    setBackgroundBrush(QBrush(backgroundColor));
}

void TargetView::paintEvent(QPaintEvent* pEvent)
{
    // Draw the scene first
    QGraphicsView::paintEvent(pEvent);

    // Let state draw custom cursor on top if needed
    if (m_mouseInView && !m_isPanning && m_pCurrentState)
    {
        QPainter painter(viewport());
        painter.setRenderHint(QPainter::Antialiasing);
        m_pCurrentState->drawCursor(&painter, m_currentMousePos, transform().m11());
    }
}

void TargetView::enterEvent(QEnterEvent* pEvent)
{
    m_mouseInView = true;
    QGraphicsView::enterEvent(pEvent);
    viewport()->update();
}

void TargetView::leaveEvent(QEvent* pEvent)
{
    m_mouseInView = false;
    QGraphicsView::leaveEvent(pEvent);
    viewport()->update();
}

