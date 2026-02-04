#pragma once

#include <QList>
#include <QPointF>

/// @brief Simple circle structure for algorithm use
///
/// Represents a circle defined by its center point and radius. Used internally
/// by the minimum enclosing circle algorithm to represent candidate solutions.
struct Circle
{
    QPointF center;  ///< Center point of the circle
    double radius = 0.0;  ///< Radius of the circle (must be non-negative)
    
    Circle() = default;
    Circle(const QPointF& c, double r) : center(c), radius(r) {}
    
    /// Check if a point lies within or on the boundary of this circle
    ///
    /// Uses floating-point comparison with an epsilon tolerance to handle
    /// numerical precision issues. A point is considered contained if its
    /// distance from the center is less than or equal to the radius plus epsilon.
    /// \param p The point to test
    /// \param epsilon Tolerance for floating-point comparison (default: 1e-9)
    /// \return True if the point is within or on the circle boundary
    bool contains(const QPointF& p, double epsilon = 1e-9) const;
};

/// @brief Implements the smallest enclosing circle algorithm
///
/// Computes the minimum enclosing circle (MEC) for a set of points, which is
/// the smallest circle that contains all given points. This is used in shot
/// group analysis to calculate group size statistics.
///
/// The implementation uses a naive O(N³) approach that tries all pairs of
/// points (as diameter endpoints) and all triples of points (as circumcircle
/// points), then selects the smallest valid circle. This approach is sufficient
/// for typical shot counts (<100). For larger datasets, Welzl's O(N) expected
/// time algorithm could be used as an optimization.
///
/// All methods are static utility functions. The class cannot be instantiated.
class MinimumEnclosingCircle
{
public:
    /// Find the smallest circle that encloses all given points
    ///
    /// Computes the minimum enclosing circle using an exhaustive search
    /// algorithm. Handles edge cases:
    /// - Empty list: returns circle with zero radius
    /// - Single point: returns circle with zero radius centered on that point
    /// - Two points: returns circle with those points as diameter endpoints
    /// - Three or more points: tries all pairs and triples to find optimal circle
    ///
    /// \param points List of points to enclose (must not be null)
    /// \return Circle structure containing the center and radius of the MEC
    static Circle findSmallestEnclosingCircle(const QList<QPointF>& points);
    
    /// Create a circle from two points (diameter endpoints)
    ///
    /// Constructs a circle where the two given points form the diameter.
    /// The center is the midpoint between the points, and the radius is
    /// half the distance between them.
    ///
    /// \param p1 First point on the circle boundary
    /// \param p2 Second point on the circle boundary (diametrically opposite p1)
    /// \return Circle with p1 and p2 as diameter endpoints
    static Circle circleFromTwoPoints(const QPointF& p1, const QPointF& p2);
    
    /// Create circumcircle from three points
    ///
    /// Calculates the circumcircle (circle passing through all three points)
    /// of the triangle formed by the three given points. If the points are
    /// collinear (within numerical precision), falls back to creating a circle
    /// from the two farthest points.
    ///
    /// \param p1 First point on the circle boundary
    /// \param p2 Second point on the circle boundary
    /// \param p3 Third point on the circle boundary
    /// \return Circle passing through all three points, or two-point circle if collinear
    static Circle circleFromThreePoints(const QPointF& p1, const QPointF& p2, const QPointF& p3);
    
    /// Check if circle encloses all points
    ///
    /// Verifies that every point in the given list lies within or on the
    /// boundary of the specified circle. Used during the search algorithm
    /// to validate candidate solutions.
    ///
    /// \param c The circle to test
    /// \param points List of points that must all be enclosed
    /// \return True if all points are contained within the circle, false otherwise
    static bool circleEnclosesAllPoints(const Circle& c, const QList<QPointF>& points);

private:
    MinimumEnclosingCircle() = delete;
};
