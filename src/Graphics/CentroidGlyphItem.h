#pragma once

#include <QGraphicsItem>
#include <QColor>

class GraphicsSettingsService;

/// @brief Plus-shaped centroid marker
/// 
/// Renders a visual marker consisting of horizontal and vertical lines forming a + shape,
/// used to indicate the centroid (geometric center) of a shot group. The glyph is centered
/// at its position and can be customized with size. Color is managed by GraphicsSettingsService.
class CentroidGlyphItem : public QGraphicsItem
{
public:
    /// Constructs a centroid glyph with the specified diameter
    ///
    /// \param diameterPixels The size (diameter) of the glyph in pixels
    /// \param pParent Optional parent graphics item
    explicit CentroidGlyphItem(double diameterPixels, QGraphicsItem* pParent = nullptr);

    /// Returns the size (diameter) of the glyph in pixels.
    double size() const;
    
    /// Sets the size (diameter) of the glyph in pixels.
    ///
    /// Triggers a geometry change notification to ensure proper scene updates.
    /// \param size The new size in pixels (must be positive).
    void setSize(double size);

    /// Returns the color of the glyph lines from the graphics settings service.
    QColor color() const;

    // QGraphicsItem interface
    
    /// Returns the bounding rectangle of the glyph.
    ///
    /// Includes padding for the line width to ensure the entire glyph
    /// is visible when rendered.
    QRectF boundingRect() const override;
    
    /// Paints the plus-shaped marker using antialiased rendering.
    ///
    /// Draws horizontal and vertical lines, forming a + shape.
    /// The lines use round cap style for smoother appearance.
    void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;

private:
    double m_size;                           ///< Size (diameter) of the glyph in pixels.
    GraphicsSettingsService& m_service;      ///< Service for graphics settings
};
