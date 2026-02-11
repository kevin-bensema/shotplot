#pragma once

#include <QGraphicsScene>
#include <QImage>
#include <QMap>
#include <Core/GroupCircle.h>

class QGraphicsPixmapItem;
class ImpactGlyphItem;
class POAGlyphItem;
class CentroidGlyphItem;
class GroupCircleItem;
class ScaleLineItem;
class ShotGroupDocument;

/// @brief QGraphicsScene for displaying target images and shot group overlays
/// 
/// Manages the visual representation of a shooting target, including the
/// background target image and various overlay elements such as impact markers,
/// point of aim indicators, group circles, and calibration lines.
/// 
/// The scene uses z-ordering to ensure proper layering:
/// - Target image: z-value -1000 (background)
/// - Group circles: z-value 50-52 (middle layer)
/// - POA glyph: z-value 90
/// - Centroid glyph: z-value 95
/// - Impact glyphs: z-value 100
/// - Scale line: z-value 200 (top layer)
/// 
/// When connected to a ShotGroupDocument, the scene automatically updates
/// visualization settings (group circle visibility, POA visibility) when the
/// document's settings change.
class TargetScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit TargetScene(QObject* pParent = nullptr);

    // Image management
    /// @brief Sets the target image as the background of the scene
    /// 
    /// Clears any existing target image before setting the new one.
    /// The scene rectangle is automatically adjusted to match the image size.
    /// The image is placed at z-value -1000 to ensure it appears behind
    /// all overlay elements.
    /// 
    /// \param image The target image to display. If null, no image is set.
    void setTargetImage(const QImage& image);
    
    /// @brief Removes the target image from the scene
    /// 
    /// Deletes the image item and resets the internal pointer.
    void clearTargetImage();
    
    /// @brief Returns the target image pixmap item
    /// 
    /// Provides typed access to the background target image item, avoiding
    /// the need to search through scene items. Returns nullptr if no image
    /// has been set.
    /// 
    /// \return The target image pixmap item, or nullptr if no image is set
    QGraphicsPixmapItem* targetImageItem() const;

    // Document connection
    /// @brief Connects the scene to a document for automatic updates
    /// 
    /// When a document is set, the scene connects to the document's
    /// impactsChanged and visualizationSettingsChanged signals to
    /// automatically update the display when document data changes.
    /// 
    /// \param pDocument The document to connect to. Can be nullptr to disconnect.
    void setDocument(ShotGroupDocument* pDocument);

    // Impact glyphs
    /// @brief Adds a new impact glyph marker to the scene
    /// 
    /// Creates and positions an ImpactGlyphItem at the specified location.
    /// The glyph is added at z-value 100, above the image but below UI overlays.
    /// 
    /// \param id Unique identifier for the impact (typically shot number)
    /// \param position Scene coordinates where the impact occurred
    /// \param diameterPixels Diameter of the impact marker in pixels
    /// \return Pointer to the created ImpactGlyphItem
    ImpactGlyphItem* addImpactGlyph(int id, const QPointF& position, double diameterPixels);
    
    /// @brief Removes an impact glyph by its ID
    /// 
    /// \param id The identifier of the impact glyph to remove
    void removeImpactGlyph(int id);
    
    /// @brief Removes all impact glyphs from the scene
    void clearImpactGlyphs();
    
    /// @brief Updates the diameter of all existing impact glyphs
    /// 
    /// Useful when the scale factor changes and all markers need
    /// to be resized proportionally.
    /// 
    /// \param diameterPixels The new diameter in pixels for all glyphs
    void updateImpactGlyphSizes(double diameterPixels);

    // Point of Aim
    /// @brief Sets the position of the Point of Aim (POA) marker
    /// 
    /// Creates the POA glyph if it doesn't exist, then positions and
    /// shows it at the specified location. The POA glyph is rendered
    /// as an X-shaped marker at z-value 90.
    /// 
    /// \param position Scene coordinates for the point of aim
    /// \param diameterPixels Diameter of the POA marker in pixels
    void setPOAGlyph(const QPointF& position, double diameterPixels);
    
    /// @brief Removes the POA glyph from the scene
    void clearPOAGlyph();
    
    /// @brief Shows or hides the POA glyph without removing it
    /// 
    /// \param visible True to show the glyph, false to hide it
    void setPOAVisible(bool visible);

    // Centroid
    /// @brief Sets the position of the centroid marker
    /// 
    /// Creates the centroid glyph if it doesn't exist, then positions and
    /// shows it at the specified location. The centroid glyph is rendered
    /// as a plus-shaped marker at z-value 95.
    /// 
    /// \param position Scene coordinates for the centroid
    /// \param diameterPixels Diameter of the centroid marker in pixels
    void setCentroidGlyph(const QPointF& position, double diameterPixels);
    
    /// @brief Removes the centroid glyph from the scene
    void clearCentroidGlyph();
    
    /// @brief Shows or hides the centroid glyph without removing it
    /// 
    /// \param visible True to show the glyph, false to hide it
    void setCentroidVisible(bool visible);

    // Group circles
    /// @brief Sets the geometry for a specific group circle type
    /// 
    /// Creates the circle if it doesn't exist, then updates its center
    /// and radius. The circle is rendered at a type-specific z-value.
    /// 
    /// \param type The type of group circle to set
    /// \param center Center point of the circle in scene coordinates
    /// \param radius Radius of the circle in scene coordinates
    void setGroupCircle(GroupCircle::Type type, const QPointF& center, double radius);
    
    /// @brief Removes all group circles from the scene
    void clearGroupCircles();
    
    /// @brief Updates the visibility of all group circles based on document settings
    void updateGroupCirclesVisibility();

    // Scale line (for calibration)
    /// @brief Shows the scale calibration line with both endpoints
    /// 
    /// Creates the scale line if it doesn't exist, then sets both
    /// start and end points and makes it visible. The line is rendered
    /// at z-value 200 (topmost layer) for visibility during calibration.
    /// 
    /// \param start Starting point of the line in scene coordinates
    /// \param end Ending point of the line in scene coordinates
    void showScaleLine(const QPointF& start, const QPointF& end);
    
    /// @brief Hides the scale line without removing it
    /// 
    /// Useful for temporarily hiding the line during calibration.
    void hideScaleLine();
    
    /// @brief Sets the starting point of the scale line
    /// 
    /// Creates the scale line if it doesn't exist and initializes
    /// both endpoints to the start position. Useful for beginning
    /// an interactive calibration where the user drags to set the end point.
    /// 
    /// \param start Starting point of the line in scene coordinates
    void setScaleLineStart(const QPointF& start);
    
    /// @brief Updates only the end point of an existing scale line
    /// 
    /// Does nothing if the scale line doesn't exist. Use this during
    /// interactive calibration when the user is dragging to set the end point.
    /// 
    /// \param end New ending point of the line in scene coordinates
    void updateScaleLineEnd(const QPointF& end);

private:
    /// @brief Updates scene visualization based on document settings
    /// 
    /// Called automatically when the connected document emits
    /// impactsChanged or visualizationSettingsChanged signals.
    /// Updates group circle visibility and POA visibility to match
    /// the document's current visualization settings.
    void updateFromDocument();

private:
    QGraphicsPixmapItem* m_pTargetImageItem = nullptr;  ///< Background target image item (z-value -1000)
    ShotGroupDocument* m_pDocument = nullptr;           ///< Connected document for automatic updates

    // Impact glyphs (keyed by impact ID)
    QMap<int, ImpactGlyphItem*> m_impactGlyphs;        ///< Map of impact ID to glyph item (z-value 100)

    // Point of Aim
    POAGlyphItem* m_pPoaGlyph = nullptr;                ///< X-shaped point of aim marker (z-value 90)

    // Centroid
    CentroidGlyphItem* m_pCentroidGlyph = nullptr;      ///< Plus-shaped centroid marker (z-value 95)

    // Group circles
    QMap<GroupCircle::Type, GroupCircleItem*> m_groupCircles; ///< Map of circle type to item (z-values 50-52)

    // Scale calibration line
    ScaleLineItem* m_pScaleLine = nullptr;               ///< Reference line for scale calibration (z-value 200)
};
