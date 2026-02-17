#pragma once

#include <QGraphicsItem>
#include <QRectF>
#include <QColor>

class CroppingToolItem : public QGraphicsItem
{
public:
    enum HandleType {
        None,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    explicit CroppingToolItem(const QRectF& imageRect, QGraphicsItem* pParent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QRectF cropRect() const { return m_cropRect; }
    void setImageRect(const QRectF& rect);
    
    // Rotate the cropping zone 90 degrees (clockwise or counter-clockwise)
    // within the new image dimensions
    void rotate90(bool clockwise);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    HandleType getHandleAt(const QPointF& pos) const;
    void updateCursor(HandleType handle);
    QRectF getHandleRect(HandleType handle) const;
    void constrainRect();

    QRectF m_imageRect;
    QRectF m_cropRect;
    HandleType m_activeHandle = None;
    
    const qreal m_handleSize = 40.0;
    const QColor m_rectColor = QColor(0, 120, 215, 50); // 20% opaque blue
    const QColor m_edgeColor = QColor(0, 120, 215, 255); // Opaque blue
};
