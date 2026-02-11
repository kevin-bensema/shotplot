#pragma once

#include <QList>
#include <QPointF>
#include "Statistics.h"
#include "ShotImpact.h"
#include "GroupCircle.h"

class ShotGroupDocument;

/// @brief Stateless service class for calculating shot group statistics
/// 
/// Provides static utility methods for computing various statistical measures
/// of shot groups, including centroid, mean radius, standard deviation, offset
/// from point of aim, and group circles (80%, 90%, and 100%).
/// 
/// All calculations are performed in pixel space. The class cannot be
/// instantiated - all methods are static utility functions.
/// 
/// The main entry point is calculate(), which accepts a document pointer
/// and returns a complete Statistics structure. Individual calculation methods
/// are also exposed for cases where only specific statistics are needed.
class StatisticsCalculator
{
public:
    /// Calculate complete statistics for a shot group
    ///
    /// Processes the shot impacts from the document and computes all available
    /// statistics, including centroid, mean radius, standard deviation, offset
    /// from point of aim (if set), and group circles.
    /// 
    /// Requirements:
    /// - At least 2 impacts are required for valid statistics
    /// - At least 3 impacts are required for 80% and 90% group circles
    /// - Point of aim must be set for offset calculation
    /// 
    /// If insufficient impacts are provided, the returned Statistics object
    /// will have valid=false. Otherwise, all calculated values are in pixels.
    ///
    /// \param pDocument Pointer to the document containing impacts and point of aim
    /// \return Statistics structure containing all calculated values, with
    ///         valid=false if fewer than 2 impacts provided
    static Statistics calculate(const ShotGroupDocument* pDocument);
    
    /// Calculate centroid (geometric center) of points
    ///
    /// Computes the arithmetic mean of all point coordinates, which represents
    /// the geometric center (centroid) of the point set. This is the average
    /// position of all shots and is used as the reference point for radial
    /// distance calculations.
    ///
    /// \param points List of points to analyze
    /// \return Centroid point (average of all coordinates), or (0,0) if empty
    static QPointF calculateCentroid(const QList<QPointF>& points);
    
    /// Calculate mean radius from centroid
    ///
    /// Computes the average distance from all points to the given centroid.
    /// This represents the typical spread of shots around the center point.
    ///
    /// \param points List of points to analyze
    /// \param centroid Reference point for distance calculations
    /// \return Average radial distance in pixels, or 0.0 if points list is empty
    static double calculateMeanRadius(const QList<QPointF>& points, const QPointF& centroid);
    
    /// Calculate standard deviation of radial distances
    ///
    /// Computes the sample standard deviation (using N-1 denominator) of the
    /// radial distances from all points to the centroid. This measures the
    /// consistency of shot placement - lower values indicate tighter groups.
    ///
    /// \param points List of points to analyze
    /// \param centroid Reference point for distance calculations
    /// \param meanRadius Pre-calculated mean radius (from calculateMeanRadius)
    /// \return Sample standard deviation in pixels, or 0.0 if fewer than 2 points
    static double calculateStandardDeviation(const QList<QPointF>& points,
                                             const QPointF& centroid,
                                             double meanRadius);
    
    /// Find smallest enclosing circle for all points (100% group)
    ///
    /// Computes the minimum enclosing circle (MEC) that contains all given
    /// points. This represents the full group size - the diameter of this
    /// circle is the maximum spread of all shots.
    ///
    /// \param points List of points to enclose
    /// \return GroupCircle containing center, radius, and indices of all points,
    ///         or invalid circle (radius=0) if points list is empty
    static GroupCircle findFullGroupCircle(const QList<QPointF>& points);
    
    /// Find smallest enclosing circle for 80% of points
    ///
    /// Computes the minimum enclosing circle that contains approximately 80%
    /// of the points, excluding the most distant shots. This provides a measure
    /// of group size that is less affected by outliers.
    ///
    /// The algorithm selects the closest points to the centroid (by count)
    /// and finds their minimum enclosing circle. This is a heuristic approach
    /// that works well for typical shot counts.
    ///
    /// \param points List of points to analyze (should have at least 3 for meaningful results)
    /// \return GroupCircle containing center, radius, and indices of included points,
    ///         or invalid circle if fewer than 2 points provided
    static GroupCircle find80PercentGroupCircle(const QList<QPointF>& points);
    
    /// Find smallest enclosing circle for 90% of points
    ///
    /// Computes the minimum enclosing circle that contains approximately 90%
    /// of the points, excluding the most distant shots. Similar to the 80%
    /// group circle but includes more shots.
    ///
    /// The algorithm selects the closest points to the centroid (by count)
    /// and finds their minimum enclosing circle. This is a heuristic approach
    /// that works well for typical shot counts.
    ///
    /// \param points List of points to analyze (should have at least 3 for meaningful results)
    /// \return GroupCircle containing center, radius, and indices of included points,
    ///         or invalid circle if fewer than 2 points provided
    static GroupCircle find90PercentGroupCircle(const QList<QPointF>& points);

private:
    // Not instantiable - static methods only
    StatisticsCalculator() = delete;
    
    /// Find smallest enclosing circle for a percentage of points
    ///
    /// Internal helper method that implements the percentage group circle
    /// algorithm. Selects the closest points to the centroid (by the specified
    /// percentage) and computes their minimum enclosing circle.
    ///
    /// The selection is done by sorting points by distance from centroid and
    /// taking the closest N points, where N = ceil(points.size() * percentage).
    /// This heuristic approach works well for typical shot counts but may not
    /// produce the globally optimal solution for all point distributions.
    ///
    /// \param points List of points to analyze
    /// \param percentage Fraction of points to include (e.g., 0.80 for 80%)
    /// \return GroupCircle for the selected subset, or invalid circle if fewer than 2 points
    static GroupCircle findPercentageGroupCircle(const QList<QPointF>& points, double percentage);
};
