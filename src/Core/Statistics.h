#pragma once

#include "GroupCircle.h"

/// @brief Contains all calculated statistics for a shot group
///
/// The Statistics struct serves as a data container for computed shot group analysis
/// metrics. It holds both statistical measures (mean radius, standard deviation, centroid)
/// and geometric group circles (80%, 90%, and 100% groups) that represent the smallest
/// enclosing circles for different subsets of shots.
///
/// All measurements are stored in pixel coordinates with fractional precision. To convert
/// to physical units (inches, MOA, mrad) for display, use UnitConverter with the
/// appropriate pixels-per-inch scale factor from the document.
///
/// Statistics are typically calculated using StatisticsCalculator::calculate(), which
/// processes a list of shot impacts and populates all fields. The struct is designed to
/// be a simple data container with no behavior - all calculation logic resides in
/// StatisticsCalculator.
///
/// @note Requirements for valid statistics:
/// - At least 2 impacts are required for valid=true and basic statistics (centroid,
///   mean radius, standard deviation, full group circle)
/// - At least 3 impacts are required for 80% and 90% group circles to be calculated
/// - If fewer than 2 impacts are provided, valid=false and all numeric values remain
///   at their default (zero) values
///
/// @note The centroid represents the geometric center (arithmetic mean) of all shot
/// positions and is used as the reference point for radial distance calculations.
/// The group circle centers may differ from the centroid, as they are determined by
/// geometric optimization of the smallest enclosing circle.
struct Statistics
{
    /// Mean radial distance from centroid to all shot impacts, in pixels
    ///
    /// Calculated as the average distance from each shot impact to the centroid.
    /// This provides a measure of overall group dispersion around the center point.
    double meanRadiusPixels = 0.0;
    
    /// Standard deviation of radial distances from centroid, in pixels
    ///
    /// Measures the variability of shot distances from the centroid. A lower standard
    /// deviation indicates more consistent grouping, while higher values suggest
    /// greater spread or irregularity in the shot pattern.
    double standardDeviationPixels = 0.0;
    
    /// Geometric center (centroid) of all shot impacts, in pixel coordinates
    ///
    /// The arithmetic mean of all shot positions, calculated as the average X and Y
    /// coordinates. This serves as the reference point for radial distance calculations
    /// and represents the "center of mass" of the shot group.
    QPointF centroid;
    
    /// Smallest enclosing circle for 100% of shots (all impacts)
    ///
    /// Represents the full group size - the smallest circle that contains all shot
    /// impacts. This is the primary group size metric used in shooting sports analysis.
    /// Always calculated when valid=true (requires >= 2 impacts).
    GroupCircle fullGroupCircle;
    
    /// Smallest enclosing circle for 80% of shots
    ///
    /// Represents the group size excluding the worst 20% of shots. This metric helps
    /// identify the core grouping pattern by filtering out outliers. Only calculated
    /// when >= 3 impacts are provided; otherwise remains at default (invalid) state.
    GroupCircle group80Circle;
    
    /// Smallest enclosing circle for 90% of shots
    ///
    /// Represents the group size excluding the worst 10% of shots. Similar to the
    /// 80% group, this helps assess consistency by removing outliers. Only calculated
    /// when >= 3 impacts are provided; otherwise remains at default (invalid) state.
    GroupCircle group90Circle;
    
    /// Indicates whether statistics were calculated with sufficient data
    ///
    /// Set to true when statistics were calculated with at least 2 impacts. When false,
    /// all numeric values should be considered invalid and should not be used for
    /// analysis or display. Check this flag before accessing any calculated values.
    bool valid = false;
    
    /// Default constructor
    ///
    /// Creates an invalid Statistics object with all numeric values initialized to
    /// zero and valid=false. Use StatisticsCalculator::calculate() to populate
    /// the structure with actual calculated values.
    Statistics() = default;
};
