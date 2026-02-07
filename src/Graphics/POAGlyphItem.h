#pragma once

#include <QGraphicsItem>
#include <QColor>

class GraphicsSettingsService;

/// @brief X-shaped point of aim marker (rotated 45 degrees)
/// 
/// Renders a visual marker consisting of two diagonal lines forming an X shape,
/// used to indicate the point of aim on a target. The glyph is centered at its
/// position and can be customized with size and color.
class POAGlyphItem : public QGraphicsItem
{
public:
    explicit POAGlyphItem(QGraphicsItem* pParent = nullptr);

    /// Returns the size (diameter) of the glyph in pixels.
    double size() const;
    
    /// Sets the size (diameter) of the glyph in pixels.
    ///
    /// Triggers a geometry change notification to ensure proper scene updates.
    /// \param size The new size in pixels (must be positive).
    void setSize(double size);

    /// Returns the color of the glyph lines from the graphics settings service.
    QColor color() const;
    
    /// Sets the color of the glyph lines (deprecated - color now managed by GraphicsSettingsService).
    /// \param color The new color for the X marker.
    /// \deprecated This method is kept for backward compatibility but has no effect.
    void setColor(const QColor& color);

    // QGraphicsItem interface
    
    /// Returns the bounding rectangle of the glyph.
    ///
    /// Includes padding for the line width to ensure the entire glyph
    /// is visible when rendered.
    QRectF boundingRect() const override;
    
    /// Paints the X-shaped marker using antialiased rendering.
    ///
    /// Draws two diagonal lines from corner to corner, forming an X shape.
    /// The lines use round cap style for smoother appearance.
    void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;

private:
    double m_size = 30.0;                    ///< Size (diameter) of the glyph in pixels.
    GraphicsSettingsService& m_service;      ///< Service for graphics settings
};
