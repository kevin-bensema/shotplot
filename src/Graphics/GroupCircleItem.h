#pragma once

#include <QGraphicsItem>
#include <QColor>

class GraphicsSettingsService;

/// @brief Semi-transparent circle overlay for group visualization
/// 
/// Renders a colored circle with semi-transparent fill and solid outline
/// to visualize shot group boundaries. The circle type determines the
/// color scheme (blue for full group, green for 80%, orange for 90%).
/// A small crosshair is drawn at the center to mark the circle's origin.
/// 
/// The item's position is set to the circle center, and the circle is
/// drawn relative to the item's origin (0, 0).
class GroupCircleItem : public QGraphicsItem
{
public:
    /// Circle type determines color and visualization purpose
    enum class Type
    {
        Full,      ///< Full group circle (blue)
        Percent80, ///< 80% group circle (green)
        Percent90  ///< 90% group circle (orange)
    };

    /// Constructs a group circle item with the specified type
    /// \param type The circle type (determines color)
    /// \param pParent Optional parent graphics item
    explicit GroupCircleItem(Type type, QGraphicsItem* pParent = nullptr);

    /// Returns the circle type
    Type circleType() const;

    /// Sets the circle geometry and updates the item position
    ///
    /// The item's position is set to the center point, and the circle
    /// is drawn centered at the item's origin. Triggers geometry change
    /// notification and repaint.
    /// \param center The center point of the circle
    /// \param radius The radius of the circle
    void setCircle(const QPointF& center, double radius);
    
    /// Returns the circle center point
    QPointF circleCenter() const;
    
    /// Returns the circle radius
    double radius() const;

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;

private:
    Type m_type;
    QPointF m_center;
    double m_radius = 0.0;
    GraphicsSettingsService& m_service;      ///< Service for graphics settings
};
