#pragma once

#include <QString>

/// @brief Utility class for converting between measurement units
/// 
/// UnitConverter provides static methods for converting between different measurement
/// units used in shooting sports: pixels, inches, MOA (Minute of Angle), and MRAD
/// (milliradians). All internal calculations in ShotPlot use pixels; this class
/// converts to and from display units for user presentation.
/// 
/// The class handles angular measurements (MOA and MRAD) which require a distance
/// parameter since these units represent angular spread that translates to different
/// linear measurements at different ranges. MOA uses the standard definition of
/// 1.047 inches per MOA at 100 yards.
/// 
/// All conversion methods return 0.0 if invalid input is provided (e.g., negative
/// or zero distance values for angular conversions, zero or negative pixelsPerInch).
/// 
/// This is a utility class with no instance state; all methods are static and
/// the constructor is deleted to prevent instantiation.
class UnitConverter
{
public:
    /// Supported display units for shot group measurements
    enum class Unit { Inches, MOA, MRAD };
    
    /// Converts pixels to inches using the provided pixels-per-inch ratio
    /// \param pixels The pixel value to convert
    /// \param pixelsPerInch The conversion ratio (must be > 0)
    /// \return The equivalent measurement in inches, or 0.0 if pixelsPerInch <= 0
    static double pixelsToInches(double pixels, double pixelsPerInch);
    
    /// Converts inches to pixels using the provided pixels-per-inch ratio
    /// \param inches The measurement in inches to convert
    /// \param pixelsPerInch The conversion ratio
    /// \return The equivalent measurement in pixels
    static double inchesToPixels(double inches, double pixelsPerInch);
    
    /// Converts inches to MOA (Minute of Angle) at the specified distance
    ///
    /// MOA is an angular measurement where 1 MOA = 1.047 inches at 100 yards.
    /// The conversion scales proportionally with distance.
    /// \param inches The linear measurement in inches
    /// \param distanceYards The target distance in yards (must be > 0)
    /// \return The equivalent measurement in MOA, or 0.0 if distanceYards <= 0
    static double inchesToMOA(double inches, double distanceYards);
    
    /// Converts inches to milliradians (MRAD) at the specified distance
    ///
    /// MRAD is an angular measurement where 1 mrad = 1/1000 of a radian.
    /// At 100 yards, 1 mrad ≈ 3.6 inches.
    /// \param inches The linear measurement in inches
    /// \param distanceYards The target distance in yards (must be > 0)
    /// \return The equivalent measurement in milliradians, or 0.0 if distanceYards <= 0
    static double inchesToMRAD(double inches, double distanceYards);
    
    /// Converts MOA (Minute of Angle) to inches at the specified distance
    ///
    /// Uses the standard MOA definition: 1 MOA = 1.047 inches at 100 yards.
    /// \param moa The angular measurement in MOA
    /// \param distanceYards The target distance in yards
    /// \return The equivalent linear measurement in inches
    static double moaToInches(double moa, double distanceYards);
    
    /// Converts milliradians (MRAD) to inches at the specified distance
    /// \param mrad The angular measurement in milliradians
    /// \param distanceYards The target distance in yards
    /// \return The equivalent linear measurement in inches
    static double mradToInches(double mrad, double distanceYards);
    
    /// Converts yards to meters
    /// \param yards The distance in yards
    /// \return The equivalent distance in meters
    static double yardsToMeters(double yards);
    
    /// Converts meters to yards
    /// \param meters The distance in meters
    /// \return The equivalent distance in yards
    static double metersToYards(double meters);
    
    /// Formats a numeric value with its unit suffix for display
    ///
    /// Formats the value using fixed-point notation with the specified precision
    /// and appends the appropriate unit suffix (", " MOA", or " mrad").
    /// \param value The numeric value to format
    /// \param unit The unit type to append
    /// \param precision The number of decimal places (default: 2)
    /// \return A formatted string like "1.25\"" or "2.50 MOA"
    static QString formatWithUnit(double value, Unit unit, int precision = 2);
    
    /// Returns the suffix string for the given unit
    /// \param unit The unit type
    /// \return The suffix string (", " MOA", or " mrad")
    static QString unitSuffix(Unit unit);
    
    /// Parses a unit string and returns the corresponding Unit enum value
    ///
    /// Case-insensitive matching. Recognizes "moa", "mrad", and "inches".
    /// Returns Unit::Inches as the default if the string doesn't match.
    /// \param str The string to parse (e.g., from settings or user input)
    /// \return The corresponding Unit value, or Unit::Inches if unrecognized
    static Unit unitFromString(const QString &str);
    
    /// Converts a Unit enum value to its string representation
    ///
    /// Returns lowercase strings: "inches", "moa", or "mrad". Suitable for
    /// storing in settings or serialization.
    /// \param unit The unit enum value
    /// \return The lowercase string representation
    static QString unitToString(Unit unit);

private:
    UnitConverter() = delete;
};
