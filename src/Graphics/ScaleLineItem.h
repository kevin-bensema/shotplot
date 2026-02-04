#pragma once

#include <QGraphicsItem>
#include <QColor>

/// @brief Bright green line for scale calibration
/// 
/// A graphics item that renders a bright green line with circular endpoints
/// used for interactive scale calibration. The line is drawn with antialiasing
/// and includes visual markers at both endpoints to aid in precise positioning
/// during calibration workflows.
/// 
/// The item automatically handles geometry change notifications and repainting
/// when the line endpoints are modified. The bounding rect includes padding
/// to account for the line width and endpoint radius.
class ScaleLineItem : public QGraphicsItem
{
public:
    /// Constructs a scale line item
    /// \param pParent Optional parent graphics item
    explicit ScaleLineItem(QGraphicsItem* pParent = nullptr);
    
    ~ScaleLineItem();

    /// Sets both endpoints of the line
    ///
    /// Updates the line geometry and triggers a repaint. Calls
    /// prepareGeometryChange() to notify the graphics scene of the change.
    /// \param start The starting point of the line
    /// \param end The ending point of the line
    void setLine(const QPointF& start, const QPointF& end);
    
    /// Sets the starting point of the line
    ///
    /// Updates the line geometry and triggers a repaint. Calls
    /// prepareGeometryChange() to notify the graphics scene of the change.
    /// \param start The new starting point
    void setStartPoint(const QPointF& start);
    
    /// Sets the ending point of the line
    ///
    /// Updates the line geometry and triggers a repaint. Calls
    /// prepareGeometryChange() to notify the graphics scene of the change.
    /// \param end The new ending point
    void setEndPoint(const QPointF& end);

    QPointF startPoint() const;
    QPointF endPoint() const;

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;

private:
    QPointF m_start;              ///< Starting point of the line
    QPointF m_end;                ///< Ending point of the line
    QColor m_color;               ///< Bright green color for visibility
    double m_lineWidth;           ///< Width of the line stroke in pixels
    double m_endpointRadius;      ///< Radius of the circular endpoint markers
};
