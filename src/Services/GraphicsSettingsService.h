#pragma once

#include <QX/QService.h>
#include <QColor>
#include <QHash>

#include <Core/GroupCircle.h>

/// Service for managing graphics colors and opacities
/// 
/// GraphicsSettingsService decouples color and opacity roles from their actual
/// values, allowing user customization through a settings dialog. The service
/// caches values in memory for performance and persists them to QSettings.
/// 
/// When settings change, the service emits settingsChanged() signal to trigger
/// redraws in connected graphics scenes.
class GraphicsSettingsService : public QService
{
    Q_OBJECT

public:
    /// Color roles for various graphics elements
    enum class ColorRole
    {
        CalibrationLine,    ///< Calibration measurement tool
        PointOfAim,         ///< Point of aim glyph
        Centroid,           ///< Centroid glyph
        Impact,             ///< Impact glyph
        FullGroupCircle,    ///< Full group circle
        Percent90Circle,    ///< 90% group circle
        Percent80Circle,    ///< 80% group circle
        Background,         ///< Background color
        ImpactCursor,       ///< Cursor for impact marking
        PointOfAimCursor    ///< Cursor for point of aim marking
    };

    /// Opacity roles for various graphics elements (0-100 scale)
    enum class OpacityRole
    {
        FullGroupCircle,    ///< Full group circle fill opacity
        Percent90Circle,    ///< 90% percentile circle fill opacity
        Percent80Circle     ///< 80% percentile circle fill opacity
    };

    explicit GraphicsSettingsService(QObject* pParent = nullptr);

    /// Returns the color for the specified role
    QColor color(ColorRole role) const;

    /// Returns the color for the specified group circle type
    QColor color(GroupCircle::Type type) const;

    /// Returns the opacity for the specified role (0-100 scale)
    int opacity(OpacityRole role) const;

    /// Returns the opacity for the specified group circle type (0-100 scale)
    int opacity(GroupCircle::Type type) const;

    /// Sets the color for the specified role and persists to settings
    void setColor(ColorRole role, const QColor& color);

    /// Sets the color for the specified group circle type and persists to settings
    void setColor(GroupCircle::Type type, const QColor& color);

    /// Sets the opacity for the specified role and persists to settings (0-100 scale)
    void setOpacity(OpacityRole role, int opacity);

    /// Sets the opacity for the specified group circle type and persists to settings (0-100 scale)
    void setOpacity(GroupCircle::Type type, int opacity);

    /// Restores all colors and opacities to default values
    void restoreDefaults();

signals:
    /// Emitted when any color or opacity setting changes
    void settingsChanged();

private:
    void loadFromSettings();
    void saveColorToSettings(ColorRole role, const QColor& color);
    void saveOpacityToSettings(OpacityRole role, int opacity);

    /// Cached colors for fast access (read from QSettings on startup)
    QHash<ColorRole, QColor> m_colors;

    /// Cached opacities for fast access (0-100 scale, read from QSettings on startup)
    QHash<OpacityRole, int> m_opacities;
};
