#include "TargetView.h"
#include <States/WorkflowState.h>

#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
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
    
    // Background
    setBackgroundBrush(QBrush(Qt::darkGray));
    
    // Drag mode - we handle this manually
    setDragMode(QGraphicsView::NoDrag);
}

void TargetView::setWorkflowState(WorkflowState* pState)
{
    m_pCurrentState = pState;
    updateCursor();
}

WorkflowState* TargetView::workflowState() const
{
    return m_pCurrentState;
}

void TargetView::zoomIn()
{
    double newZoom = m_zoomFactor * kZoomStep;
    if (newZoom <= kMaxZoom)
    {
        scale(kZoomStep, kZoomStep);
        m_zoomFactor = newZoom;
    }
}

void TargetView::zoomOut()
{
    double newZoom = m_zoomFactor / kZoomStep;
    if (newZoom >= kMinZoom)
    {
        scale(1.0 / kZoomStep, 1.0 / kZoomStep);
        m_zoomFactor = newZoom;
    }
}

void TargetView::zoomFit()
{
    if (!scene()) return;
    
    // Reset transform
    resetTransform();
    m_zoomFactor = 1.0;
    
    // Fit scene in view
    QRectF sceneRect = scene()->sceneRect();
    if (!sceneRect.isEmpty())
    {
        // Find the target image item to get its rect
        QRectF imageRect;
        for (auto* item : scene()->items())
        {
            if (auto* pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(item))
            {
                imageRect = pixmapItem->sceneBoundingRect();
                break;
            }
        }

        if (!imageRect.isEmpty())
        {
            fitInView(imageRect, Qt::KeepAspectRatio);
        }
        else
        {
            fitInView(sceneRect, Qt::KeepAspectRatio);
        }
        
        // Calculate actual zoom factor
        QTransform t = transform();
        m_zoomFactor = t.m11();  // Assuming uniform scaling
    }
}

void TargetView::setZoomFactor(double factor)
{
    if (factor < kMinZoom || factor > kMaxZoom) return;
    
    double scaleFactor = factor / m_zoomFactor;
    scale(scaleFactor, scaleFactor);
    m_zoomFactor = factor;
}

double TargetView::zoomFactor() const
{
    return m_zoomFactor;
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
    
    // Left button - let state handle it on release (to distinguish click from drag)
    pEvent->accept();
}

void TargetView::mouseMoveEvent(QMouseEvent* pEvent)
{
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
        return;
    }
    
    // Check if we've dragged far enough to start panning
    if (pEvent->buttons() & Qt::LeftButton)
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
    
    pEvent->accept();
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
    
    pEvent->accept();
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
