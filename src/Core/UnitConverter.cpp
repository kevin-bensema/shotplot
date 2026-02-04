#include "UnitConverter.h"

#include <cmath>

namespace
{
    /// MOA conversion factor: 1 MOA = 1.047 inches at 100 yards
    ///
    /// This is the standard definition used in shooting sports. Some approximations
    /// use 1.0 inch per MOA, but this class uses the exact value.
    constexpr double kMoaFactor = 1.047;  ///< inches per MOA at 100 yards
    
    /// Conversion factor from radians to milliradians
    constexpr double kMradPerRadian = 1000.0;
    
    /// Conversion factor from yards to inches
    constexpr double kInchesPerYard = 36.0;
    
    /// Conversion factor from yards to meters
    constexpr double kYardsToMetersFactor = 0.9144;
}

double UnitConverter::pixelsToInches(double pixels, double pixelsPerInch)
{
    if (pixelsPerInch <= 0.0)
    {
        return 0.0;
    }
    return pixels / pixelsPerInch;
}

double UnitConverter::inchesToPixels(double inches, double pixelsPerInch)
{
    return inches * pixelsPerInch;
}

double UnitConverter::inchesToMOA(double inches, double distanceYards)
{
    if (distanceYards <= 0.0)
    {
        return 0.0;
    }
    // MOA = inches / (distance_yards / 100) / kMoaFactor
    // At 100 yards, 1 MOA = 1.047 inches
    return inches / (distanceYards / 100.0) / kMoaFactor;
}

double UnitConverter::inchesToMRAD(double inches, double distanceYards)
{
    if (distanceYards <= 0.0)
    {
        return 0.0;
    }
    // Convert yards to inches, then calculate angle in radians, then to milliradians
    double distanceInches = distanceYards * kInchesPerYard;
    double radians = inches / distanceInches;
    return radians * kMradPerRadian;
}

double UnitConverter::moaToInches(double moa, double distanceYards)
{
    // At 100 yards, 1 MOA = 1.047 inches
    return moa * (distanceYards / 100.0) * kMoaFactor;
}

double UnitConverter::mradToInches(double mrad, double distanceYards)
{
    double distanceInches = distanceYards * kInchesPerYard;
    double radians = mrad / kMradPerRadian;
    return radians * distanceInches;
}

double UnitConverter::yardsToMeters(double yards)
{
    return yards * kYardsToMetersFactor;
}

double UnitConverter::metersToYards(double meters)
{
    return meters / kYardsToMetersFactor;
}

QString UnitConverter::formatWithUnit(double value, Unit unit, int precision)
{
    QString suffix = unitSuffix(unit);
    return QString::number(value, 'f', precision) + suffix;
}

QString UnitConverter::unitSuffix(Unit unit)
{
    switch (unit)
    {
        case Unit::Inches:
            return "\"";
        case Unit::MOA:
            return " MOA";
        case Unit::MRAD:
            return " mrad";
    }
    return "";
}

UnitConverter::Unit UnitConverter::unitFromString(const QString &str)
{
    QString lower = str.toLower();
    if (lower == "moa")
    {
        return Unit::MOA;
    }
    else if (lower == "mrad")
    {
        return Unit::MRAD;
    }
    return Unit::Inches;
}

QString UnitConverter::unitToString(Unit unit)
{
    switch (unit)
    {
        case Unit::Inches:
            return "inches";
        case Unit::MOA:
            return "moa";
        case Unit::MRAD:
            return "mrad";
    }
    return "inches";
}
