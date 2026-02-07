#include "GraphicsSettingsService.h"
#include <QSettings>

namespace
{
    // QSettings keys using dot-notation
    const QString kColorCalibrationLineKey = QStringLiteral("Graphics.Colors.CalibrationLine");
    const QString kColorPointOfAimKey = QStringLiteral("Graphics.Colors.PointOfAim");
    const QString kColorImpactKey = QStringLiteral("Graphics.Colors.Impact");
    const QString kColorFullGroupCircleKey = QStringLiteral("Graphics.Colors.FullGroupCircle");
    const QString kColorPercent90CircleKey = QStringLiteral("Graphics.Colors.Percent90Circle");
    const QString kColorPercent80CircleKey = QStringLiteral("Graphics.Colors.Percent80Circle");
    const QString kColorBackgroundKey = QStringLiteral("Graphics.Colors.Background");
    
    const QString kOpacityFullGroupCircleKey = QStringLiteral("Graphics.Opacities.FullGroupCircle");
    const QString kOpacityPercent90CircleKey = QStringLiteral("Graphics.Opacities.Percent90Circle");
    const QString kOpacityPercent80CircleKey = QStringLiteral("Graphics.Opacities.Percent80Circle");

    // Default color values
    const QColor kDefaultCalibrationLineColor(0, 255, 0);           // Bright green
    const QColor kDefaultPointOfAimColor(0, 0, 139);                // Dark blue
    const QColor kDefaultImpactColor(Qt::red);                      // Red
    const QColor kDefaultFullGroupCircleColor(0, 102, 204);         // Blue
    const QColor kDefaultPercent90CircleColor(255, 153, 51);        // Orange
    const QColor kDefaultPercent80CircleColor(0, 204, 102);         // Green
    const QColor kDefaultBackgroundColor(30, 30, 30);               // Almost black

    // Default opacity values (0-100 scale)
    constexpr int kDefaultFullGroupCircleOpacity = 16;              // 40/255 ≈ 16%
    constexpr int kDefaultPercent90CircleOpacity = 16;
    constexpr int kDefaultPercent80CircleOpacity = 16;

    QString colorRoleToKey(GraphicsSettingsService::ColorRole role)
    {
        switch (role)
        {
            case GraphicsSettingsService::ColorRole::CalibrationLine:
                return kColorCalibrationLineKey;
            case GraphicsSettingsService::ColorRole::PointOfAim:
                return kColorPointOfAimKey;
            case GraphicsSettingsService::ColorRole::Impact:
                return kColorImpactKey;
            case GraphicsSettingsService::ColorRole::FullGroupCircle:
                return kColorFullGroupCircleKey;
            case GraphicsSettingsService::ColorRole::Percent90Circle:
                return kColorPercent90CircleKey;
            case GraphicsSettingsService::ColorRole::Percent80Circle:
                return kColorPercent80CircleKey;
            case GraphicsSettingsService::ColorRole::Background:
                return kColorBackgroundKey;
        }
        return QString();
    }

    QString opacityRoleToKey(GraphicsSettingsService::OpacityRole role)
    {
        switch (role)
        {
            case GraphicsSettingsService::OpacityRole::FullGroupCircle:
                return kOpacityFullGroupCircleKey;
            case GraphicsSettingsService::OpacityRole::Percent90Circle:
                return kOpacityPercent90CircleKey;
            case GraphicsSettingsService::OpacityRole::Percent80Circle:
                return kOpacityPercent80CircleKey;
        }
        return QString();
    }

    QColor defaultColorForRole(GraphicsSettingsService::ColorRole role)
    {
        switch (role)
        {
            case GraphicsSettingsService::ColorRole::CalibrationLine:
                return kDefaultCalibrationLineColor;
            case GraphicsSettingsService::ColorRole::PointOfAim:
                return kDefaultPointOfAimColor;
            case GraphicsSettingsService::ColorRole::Impact:
                return kDefaultImpactColor;
            case GraphicsSettingsService::ColorRole::FullGroupCircle:
                return kDefaultFullGroupCircleColor;
            case GraphicsSettingsService::ColorRole::Percent90Circle:
                return kDefaultPercent90CircleColor;
            case GraphicsSettingsService::ColorRole::Percent80Circle:
                return kDefaultPercent80CircleColor;
            case GraphicsSettingsService::ColorRole::Background:
                return kDefaultBackgroundColor;
        }
        return QColor();
    }

    int defaultOpacityForRole(GraphicsSettingsService::OpacityRole role)
    {
        switch (role)
        {
            case GraphicsSettingsService::OpacityRole::FullGroupCircle:
                return kDefaultFullGroupCircleOpacity;
            case GraphicsSettingsService::OpacityRole::Percent90Circle:
                return kDefaultPercent90CircleOpacity;
            case GraphicsSettingsService::OpacityRole::Percent80Circle:
                return kDefaultPercent80CircleOpacity;
        }
        return 0;
    }
}

GraphicsSettingsService::GraphicsSettingsService(QObject* pParent)
    : QService(pParent)
{
    loadFromSettings();
}

QColor GraphicsSettingsService::color(ColorRole role) const
{
    return m_colors.value(role);
}

int GraphicsSettingsService::opacity(OpacityRole role) const
{
    return m_opacities.value(role);
}

void GraphicsSettingsService::setColor(ColorRole role, const QColor& color)
{
    if (m_colors.value(role) != color)
    {
        m_colors[role] = color;
        saveColorToSettings(role, color);
        emit settingsChanged();
    }
}

void GraphicsSettingsService::setOpacity(OpacityRole role, int opacity)
{
    // Clamp to valid range
    opacity = qBound(0, opacity, 100);
    
    if (m_opacities.value(role) != opacity)
    {
        m_opacities[role] = opacity;
        saveOpacityToSettings(role, opacity);
        emit settingsChanged();
    }
}

void GraphicsSettingsService::restoreDefaults()
{
    // Restore all colors
    m_colors[ColorRole::CalibrationLine] = kDefaultCalibrationLineColor;
    m_colors[ColorRole::PointOfAim] = kDefaultPointOfAimColor;
    m_colors[ColorRole::Impact] = kDefaultImpactColor;
    m_colors[ColorRole::FullGroupCircle] = kDefaultFullGroupCircleColor;
    m_colors[ColorRole::Percent90Circle] = kDefaultPercent90CircleColor;
    m_colors[ColorRole::Percent80Circle] = kDefaultPercent80CircleColor;
    m_colors[ColorRole::Background] = kDefaultBackgroundColor;

    // Restore all opacities
    m_opacities[OpacityRole::FullGroupCircle] = kDefaultFullGroupCircleOpacity;
    m_opacities[OpacityRole::Percent90Circle] = kDefaultPercent90CircleOpacity;
    m_opacities[OpacityRole::Percent80Circle] = kDefaultPercent80CircleOpacity;

    // Save all to settings
    QSettings settings;
    for (auto it = m_colors.constBegin(); it != m_colors.constEnd(); ++it)
    {
        saveColorToSettings(it.key(), it.value());
    }
    for (auto it = m_opacities.constBegin(); it != m_opacities.constEnd(); ++it)
    {
        saveOpacityToSettings(it.key(), it.value());
    }

    emit settingsChanged();
}

void GraphicsSettingsService::loadFromSettings()
{
    QSettings settings;

    // Load colors with defaults
    m_colors[ColorRole::CalibrationLine] = settings.value(
        kColorCalibrationLineKey, kDefaultCalibrationLineColor).value<QColor>();
    m_colors[ColorRole::PointOfAim] = settings.value(
        kColorPointOfAimKey, kDefaultPointOfAimColor).value<QColor>();
    m_colors[ColorRole::Impact] = settings.value(
        kColorImpactKey, kDefaultImpactColor).value<QColor>();
    m_colors[ColorRole::FullGroupCircle] = settings.value(
        kColorFullGroupCircleKey, kDefaultFullGroupCircleColor).value<QColor>();
    m_colors[ColorRole::Percent90Circle] = settings.value(
        kColorPercent90CircleKey, kDefaultPercent90CircleColor).value<QColor>();
    m_colors[ColorRole::Percent80Circle] = settings.value(
        kColorPercent80CircleKey, kDefaultPercent80CircleColor).value<QColor>();
    m_colors[ColorRole::Background] = settings.value(
        kColorBackgroundKey, kDefaultBackgroundColor).value<QColor>();

    // Load opacities with defaults
    m_opacities[OpacityRole::FullGroupCircle] = settings.value(
        kOpacityFullGroupCircleKey, kDefaultFullGroupCircleOpacity).toInt();
    m_opacities[OpacityRole::Percent90Circle] = settings.value(
        kOpacityPercent90CircleKey, kDefaultPercent90CircleOpacity).toInt();
    m_opacities[OpacityRole::Percent80Circle] = settings.value(
        kOpacityPercent80CircleKey, kDefaultPercent80CircleOpacity).toInt();
}

void GraphicsSettingsService::saveColorToSettings(ColorRole role, const QColor& color)
{
    QSettings settings;
    settings.setValue(colorRoleToKey(role), color);
}

void GraphicsSettingsService::saveOpacityToSettings(OpacityRole role, int opacity)
{
    QSettings settings;
    settings.setValue(opacityRoleToKey(role), opacity);
}
