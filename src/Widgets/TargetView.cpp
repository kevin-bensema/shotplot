#include "TargetView.h"
#include "States/WorkflowState.h"

#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <cmath>

TargetView::TargetView(QWidget *parent)
    : QGraphicsView(parent)
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

void TargetView::setWorkflowState(WorkflowState* state)
{
    m_currentState = state;
    updateCursor();
}

void TargetView::zoomIn()
{
    double newZoom = m_zoomFactor * ZOOM_STEP;
    if (newZoom <= MAX_ZOOM)
    {
        scale(ZOOM_STEP, ZOOM_STEP);
        m_zoomFactor = newZoom;
    }
}

void TargetView::zoomOut()
{
    double newZoom = m_zoomFactor / ZOOM_STEP;
    if (newZoom >= MIN_ZOOM)
    {
        scale(1.0 / ZOOM_STEP, 1.0 / ZOOM_STEP);
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
        fitInView(sceneRect, Qt::KeepAspectRatio);
        
        // Calculate actual zoom factor
        QTransform t = transform();
        m_zoomFactor = t.m11();  // Assuming uniform scaling
    }
}

void TargetView::setZoomFactor(double factor)
{
    if (factor < MIN_ZOOM || factor > MAX_ZOOM) return;
    
    double scaleFactor = factor / m_zoomFactor;
    scale(scaleFactor, scaleFactor);
    m_zoomFactor = factor;
}

void TargetView::wheelEvent(QWheelEvent *event)
{
    // Zoom with mouse wheel
    if (event->angleDelta().y() > 0)
    {
        zoomIn();
    }
    else
    {
        zoomOut();
    }
    event->accept();
}

void TargetView::mousePressEvent(QMouseEvent *event)
{
    m_mousePressPos = event->pos();
    m_lastMousePos = event->pos();
    
    // Middle button or Ctrl+left always pans
    if (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ControlModifier)))
    {
        m_isPanning = true;
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    
    // Right button - context menu or state action
    if (event->button() == Qt::RightButton && m_currentState)
    {
        QPointF scenePos = mapToScene(event->pos());
        m_currentState->handleRightClick(scenePos);
        event->accept();
        return;
    }
    
    // Left button - let state handle it on release (to distinguish click from drag)
    event->accept();
}

void TargetView::mouseMoveEvent(QMouseEvent *event)
{
    // Emit position for status bar
    QPointF scenePos = mapToScene(event->pos());
    emit mousePositionChanged(scenePos);
    
    if (m_isPanning)
    {
        // Pan the view
        QPointF delta = event->pos() - m_lastMousePos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_lastMousePos = event->pos();
        event->accept();
        return;
    }
    
    // Check if we've dragged far enough to start panning
    if (event->buttons() & Qt::LeftButton)
    {
        QPointF delta = event->pos() - m_mousePressPos;
        double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
        
        if (distance >= CLICK_THRESHOLD && !m_isPanning)
        {
            // Start panning
            m_isPanning = true;
            setCursor(Qt::ClosedHandCursor);
        }
    }
    
    // Delegate to state for hover effects, line drawing, etc.
    if (m_currentState)
    {
        m_currentState->handleMouseMove(scenePos);
    }
    
    event->accept();
}

void TargetView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isPanning)
    {
        m_isPanning = false;
        updateCursor();
        event->accept();
        return;
    }
    
    // Check for click vs drag
    QPointF delta = event->pos() - m_mousePressPos;
    double distance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
    
    if (distance < CLICK_THRESHOLD && event->button() == Qt::LeftButton)
    {
        // This was a click - delegate to state
        if (m_currentState)
        {
            QPointF scenePos = mapToScene(event->pos());
            m_currentState->handleMouseClick(scenePos);
        }
    }
    
    event->accept();
}

void TargetView::updateCursor()
{
    if (m_currentState)
    {
        setCursor(m_currentState->cursor());
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}
