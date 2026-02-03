#pragma once

#include <QGraphicsItem>
#include <QColor>

/// @brief Segmented circle glyph for shot impact markers
/// 
/// Renders a circle with gaps at N, S, E, W cardinal directions,
/// with a shot number displayed in the center. The circle is drawn
/// as four arc segments separated by gaps, creating a distinctive
/// visual marker for each shot impact on the target.
/// 
/// The item is selectable and automatically updates its visual
/// representation when properties change. The shot number is rendered
/// in white text with a black outline for visibility against any
/// background.
class ImpactGlyphItem : public QGraphicsItem
{
public:
    /// @brief Constructs an impact glyph item
    /// 
    /// \param shotNumber The shot number to display in the center
    /// \param diameter The diameter of the circle in scene coordinates
    /// \param parent Optional parent graphics item
    explicit ImpactGlyphItem(int shotNumber, double diameter, QGraphicsItem *parent = nullptr);
    ~ImpactGlyphItem();

    int shotNumber() const { return m_shotNumber; }
    
    /// @brief Sets the shot number and triggers a repaint
    /// 
    /// \param number The shot number to display
    void setShotNumber(int number);

    double diameter() const { return m_diameter; }
    
    /// @brief Sets the circle diameter and updates geometry
    /// 
    /// Calls prepareGeometryChange() before updating the diameter
    /// to ensure proper scene updates.
    /// 
    /// \param diameter The new diameter in scene coordinates
    void setDiameter(double diameter);

    QColor color() const { return m_color; }
    
    /// @brief Sets the circle color and triggers a repaint
    /// 
    /// \param color The color for the circle outline
    void setColor(const QColor &color);

    // QGraphicsItem interface
    
    /// @brief Returns the bounding rectangle of the glyph
    /// 
    /// Includes padding for the line width to ensure proper
    /// rendering and hit testing.
    QRectF boundingRect() const override;
    
    /// @brief Paints the segmented circle and shot number
    /// 
    /// Draws four arc segments with gaps at cardinal directions
    /// (N, S, E, W) and renders the shot number in the center
    /// with white fill and black outline.
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int m_shotNumber;              ///< Shot number displayed in center
    double m_diameter;             ///< Circle diameter in scene coordinates
    QColor m_color;                ///< Color of the circle outline
    double m_lineWidth = 2.0;      ///< Width of the circle outline in pixels
    double m_gapAngle = 15.0;      ///< Angular size of each gap in degrees
};
