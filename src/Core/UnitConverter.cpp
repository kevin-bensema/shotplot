#include "UnitConverter.h"

#include <cmath>

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
    // MOA = inches / (distance_yards / 100) / MOA_FACTOR
    // At 100 yards, 1 MOA = 1.047 inches
    return inches / (distanceYards / 100.0) / MOA_FACTOR;
}

double UnitConverter::inchesToMRAD(double inches, double distanceYards)
{
    if (distanceYards <= 0.0)
    {
        return 0.0;
    }
    // Convert yards to inches, then calculate angle in radians, then to milliradians
    double distanceInches = distanceYards * INCHES_PER_YARD;
    double radians = inches / distanceInches;
    return radians * MRAD_PER_RADIAN;
}

double UnitConverter::moaToInches(double moa, double distanceYards)
{
    // At 100 yards, 1 MOA = 1.047 inches
    return moa * (distanceYards / 100.0) * MOA_FACTOR;
}

double UnitConverter::mradToInches(double mrad, double distanceYards)
{
    double distanceInches = distanceYards * INCHES_PER_YARD;
    double radians = mrad / MRAD_PER_RADIAN;
    return radians * distanceInches;
}

double UnitConverter::yardsToMeters(double yards)
{
    return yards * 0.9144;
}

double UnitConverter::metersToYards(double meters)
{
    return meters / 0.9144;
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
