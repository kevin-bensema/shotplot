#pragma once

#include <QString>
#include <QPointF>

/// @brief Represents a single shot impact location on a target
/// 
/// ShotImpact is a lightweight data structure that stores the location and metadata
/// for a single bullet impact point. It is used throughout the ShotPlot application
/// to represent individual shots in a shooting session.
/// 
/// All coordinates are stored in pixel space (fractional/float values). Physical
/// measurements (e.g., distance from point of aim, group size) are calculated
/// on-demand using the scale factor (pixelsPerInch) stored in ShotGroupDocument.
/// 
/// Each impact has a unique identifier that is assigned when the impact is added
/// to the document. This ID is used for tracking and can be used to reference
/// specific impacts in the UI or for serialization.
/// 
/// The position can be accessed either as individual x/y coordinates or as a
/// QPointF for convenience when working with Qt graphics APIs.
/// 
/// @note This is a header-only struct with no corresponding .cpp file. All
///       methods are inline for performance and simplicity.
struct ShotImpact
{
    int id = 0;              ///< Unique identifier for this impact (assigned by ShotGroupDocument)
    double x = 0.0;          ///< X coordinate in pixel space (fractional, can be sub-pixel)
    double y = 0.0;          ///< Y coordinate in pixel space (fractional, can be sub-pixel)
    
    /// Default constructor creates an impact at origin (0, 0) with id 0
    ShotImpact() = default;
    
    /// @brief Constructs a ShotImpact with the specified parameters
    /// 
    /// \param id Unique identifier for this impact
    /// \param x X coordinate in pixel space
    /// \param y Y coordinate in pixel space
    /// \param notes Optional notes string (defaults to empty QString)
    ShotImpact(int id, double x, double y)
        : id(id), x(x), y(y) {}
    
    /// @brief Returns the position as a QPointF
    /// 
    /// Convenience method for working with Qt graphics APIs that expect QPointF.
    /// 
    /// \return QPointF containing the (x, y) coordinates
    QPointF position() const { return QPointF(x, y); }
    
    /// @brief Sets the position from a QPointF
    /// 
    /// Updates both x and y coordinates from the provided QPointF. Useful when
    /// receiving position data from Qt graphics APIs.
    /// 
    /// \param pos QPointF containing the new (x, y) coordinates
    void setPosition(const QPointF& pos)
    {
        x = pos.x();
        y = pos.y();
    }
};
