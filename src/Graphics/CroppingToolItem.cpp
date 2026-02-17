#include "CroppingToolItem.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QGraphicsScene>

CroppingToolItem::CroppingToolItem(const QRectF& imageRect, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
    , m_imageRect(imageRect)
    , m_cropRect(imageRect)
{
    setAcceptHoverEvents(true);
    setZValue(1000); // Ensure it's on top
}

QRectF CroppingToolItem::boundingRect() const
{
    return m_imageRect.adjusted(-m_handleSize, -m_handleSize, m_handleSize, m_handleSize);
}

void CroppingToolItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw 10% opaque blue rectangle
    painter->setBrush(m_rectColor);
    painter->setPen(Qt::NoPen);
    painter->drawRect(m_cropRect);

    // Draw opaque blue edges
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(m_edgeColor, 2));
    painter->drawRect(m_cropRect);

    // Draw handles
    painter->setBrush(m_edgeColor);
    painter->drawEllipse(getHandleRect(TopLeft));
    painter->drawEllipse(getHandleRect(TopRight));
    painter->drawEllipse(getHandleRect(BottomLeft));
    painter->drawEllipse(getHandleRect(BottomRight));
}

void CroppingToolItem::setImageRect(const QRectF& rect)
{
    prepareGeometryChange();
    m_imageRect = rect;
    m_cropRect = m_cropRect.intersected(m_imageRect);
    if (m_cropRect.isEmpty()) 
    {
        m_cropRect = m_imageRect;
    }
    update();
}

void CroppingToolItem::rotate90(bool clockwise)
{
    prepareGeometryChange();
    
    // The image dimensions swap
    qreal newWidth = m_imageRect.height();
    qreal newHeight = m_imageRect.width();
    m_imageRect = QRectF(0, 0, newWidth, newHeight);

    // Map old crop rect to new coordinates
    QRectF oldCrop = m_cropRect;
    qreal oldW = oldCrop.width();
    qreal oldH = oldCrop.height();
    qreal oldX = oldCrop.x();
    qreal oldY = oldCrop.y();

    if (clockwise)
    {
        // (x, y) -> (newW - y - oldH, x)
        m_cropRect = QRectF(newWidth - oldY - oldH, oldX, oldH, oldW);
    }
    else
    {
        // (x, y) -> (y, newH - x - oldW)
        m_cropRect = QRectF(oldY, newHeight - oldX - oldW, oldH, oldW);
    }
    
    constrainRect();
    update();
}

void CroppingToolItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_activeHandle = getHandleAt(event->pos());
    if (m_activeHandle != None) 
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void CroppingToolItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_activeHandle == None) return;

    QPointF pos = event->pos();
    
    // Constrain pos to image rect
    pos.setX(qBound(m_imageRect.left(), pos.x(), m_imageRect.right()));
    pos.setY(qBound(m_imageRect.top(), pos.y(), m_imageRect.bottom()));

    switch (m_activeHandle) 
    {
    case TopLeft:
        m_cropRect.setTopLeft(pos);
        break;
    case TopRight:
        m_cropRect.setTopRight(pos);
        break;
    case BottomLeft:
        m_cropRect.setBottomLeft(pos);
        break;
    case BottomRight:
        m_cropRect.setBottomRight(pos);
        break;
    default:
        break;
    }

    constrainRect();
    update();
}

void CroppingToolItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    m_activeHandle = None;
    updateCursor(None);
    event->accept();
}

void CroppingToolItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    updateCursor(getHandleAt(event->pos()));
}

CroppingToolItem::HandleType CroppingToolItem::getHandleAt(const QPointF& pos) const
{
    if (getHandleRect(TopLeft).contains(pos)) return TopLeft;
    if (getHandleRect(TopRight).contains(pos)) return TopRight;
    if (getHandleRect(BottomLeft).contains(pos)) return BottomLeft;
    if (getHandleRect(BottomRight).contains(pos)) return BottomRight;
    return None;
}

void CroppingToolItem::updateCursor(HandleType handle)
{
    switch (handle) 
    {
    case TopLeft:
    case BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case TopRight:
    case BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
        break;
    }
}

QRectF CroppingToolItem::getHandleRect(HandleType handle) const
{
    QPointF center;
    switch (handle) 
    {
    case TopLeft: center = m_cropRect.topLeft(); break;
    case TopRight: center = m_cropRect.topRight(); break;
    case BottomLeft: center = m_cropRect.bottomLeft(); break;
    case BottomRight: center = m_cropRect.bottomRight(); break;
    default: return QRectF();
    }
    return QRectF(center.x() - m_handleSize / 2.0, center.y() - m_handleSize / 2.0, m_handleSize, m_handleSize);
}

void CroppingToolItem::constrainRect()
{
    // Ensure width and height are positive
    if (m_cropRect.width() < 0) 
    {
        if (m_activeHandle == TopLeft || m_activeHandle == BottomLeft) 
        {
             m_cropRect.setLeft(m_cropRect.right());
        } 
        else
        {
             m_cropRect.setRight(m_cropRect.left());
        }
    }
    if (m_cropRect.height() < 0) 
    {
        if (m_activeHandle == TopLeft || m_activeHandle == TopRight) 
        {
             m_cropRect.setTop(m_cropRect.bottom());
        } 
        else
        {
             m_cropRect.setBottom(m_cropRect.top());
        }
    }

    // Keep within image bounds
    m_cropRect = m_cropRect.normalized().intersected(m_imageRect);
}
