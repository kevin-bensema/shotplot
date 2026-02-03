#pragma once

#include <QPointF>
#include <QList>

/// @brief Represents a group circle (smallest enclosing circle for a set of shots)
/// 
/// A GroupCircle represents the smallest circle that encloses a specified subset
/// of shot impacts. This is used in shot group analysis to calculate group size
/// statistics such as:
/// - Full group (100%): smallest circle enclosing all shots
/// - 80% group: smallest circle enclosing 80% of shots
/// - 90% group: smallest circle enclosing 90% of shots
/// 
/// The group size is defined as the diameter of the smallest enclosing circle,
/// which is calculated using the minimum enclosing circle algorithm. This differs
/// from simple statistical measures like extreme spread or mean radius.
/// 
/// All measurements (center coordinates and radius) are stored in pixel coordinates
/// with fractional precision. To convert to physical units (inches, MOA, mrad),
/// use UnitConverter with the appropriate pixels-per-inch scale factor.
/// 
/// @note The center of the group circle may not coincide with the centroid
/// (geometric center) of the shots. The centroid is used for mean radius
/// calculations, while the group circle center is determined by the geometric
/// optimization of the smallest enclosing circle.
struct GroupCircle
{
    QPointF center;              ///< Circle center in pixel coordinates
    double radiusPixels = 0.0;   ///< Circle radius in pixels
    QList<int> shotIndices;      ///< Indices of shots enclosed by this circle
    
    /// Default constructor
    ///
    /// Creates an invalid group circle with zero radius. Use isValid() to check
    /// if the circle has been properly initialized.
    GroupCircle() = default;
    
    /// Construct a group circle with specified parameters
    ///
    /// \param center The center point of the circle in pixel coordinates
    /// \param radius The radius of the circle in pixels (must be non-negative)
    /// \param indices Optional list of shot indices enclosed by this circle.
    ///                If empty, the circle represents all shots in the group.
    GroupCircle(const QPointF &center, double radius, const QList<int> &indices = {})
        : center(center), radiusPixels(radius), shotIndices(indices) {}
    
    /// Check if this group circle is valid
    ///
    /// A group circle is considered valid if it has a positive radius.
    /// Invalid circles (radius = 0) typically indicate edge cases such as
    /// empty shot lists or single-point groups.
    /// \return True if radiusPixels > 0.0, false otherwise
    bool isValid() const { return radiusPixels > 0.0; }
    
    /// Calculate the diameter of the circle
    ///
    /// Returns the diameter, which is twice the radius. The diameter represents
    /// the group size metric used in shooting sports analysis.
    /// \return The diameter in pixels (radiusPixels * 2.0)
    double diameterPixels() const { return radiusPixels * 2.0; }
};
