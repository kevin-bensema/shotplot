#pragma once

#include <QObject>
#include <QImage>
#include <QPointF>
#include <QDate>
#include <QList>
#include <QString>

#include "ShotImpact.h"
#include "Statistics.h"

/// @brief Central data model for a ShotPlot session
/// 
/// ShotGroupDocument is the core data container for a shooting session. It stores
/// the target image, calibration data, shot impacts, metadata, and visualization
/// settings. All coordinate data is stored in pixel space; physical measurements
/// are calculated on-demand using the scale factor (pixelsPerInch).
/// 
/// The document manages a workflow state progression:
/// 1. Set caliber (bullet diameter)
/// 2. Set scale factor (pixels per inch calibration)
/// 3. Mark point of aim (optional)
/// 4. Mark shot impacts
/// 5. Visualize statistics
/// 
/// Statistics are automatically recalculated when impacts or scale factor change.
/// The document emits statisticsChanged() signal when statistics are updated.
/// 
/// The document emits Qt signals for data changes, allowing UI components to
/// react to modifications. Use isDirty() to track unsaved changes.
class ShotGroupDocument : public QObject
{
    Q_OBJECT

public:
    /// Distance units for target range measurement
    enum class DistanceUnit { Yards, Meters };
    
    /// @brief Configuration for the statistics plaque overlay
    /// 
    /// Controls the position, size, and content of the statistics text overlay
    /// displayed on the target visualization.
    struct PlaqueConfig
    {
        bool enabled = false;      ///< Whether the plaque is visible
        int x = 50;                ///< X position in pixels
        int y = 50;                ///< Y position in pixels
        int width = 300;           ///< Width in pixels
        int height = 200;          ///< Height in pixels
        QString title = "Shot Group Statistics";  ///< Plaque title text
        QString formatString;      ///< Format string for statistics display
    };

    explicit ShotGroupDocument(QObject *parent = nullptr);
    ~ShotGroupDocument();

    // ===== Image Data =====
    QImage targetImage() const;
    /// Sets the target image and marks the document as dirty
    void setTargetImage(const QImage &image);

    // ===== Calibration Data =====
    double bulletDiameter() const;
    void setBulletDiameter(double diameter);
    /// Returns true if bullet diameter has been set (required for workflow step 2)
    bool hasCaliberSet() const;

    double pixelsPerInch() const;
    void setPixelsPerInch(double ppi);
    /// Returns true if scale factor has been set (required for workflow steps 3-4)
    bool hasScaleFactorSet() const;

    // ===== Point of Aim =====
    QPointF pointOfAim() const;
    /// Sets the point of aim and marks it as configured
    void setPointOfAim(const QPointF &poa);
    /// Clears the point of aim and marks it as unset
    void clearPointOfAim();
    bool hasPointOfAimSet() const;

    // ===== Session Parameters =====
    double targetDistance() const;
    void setTargetDistance(double distance);

    DistanceUnit distanceUnit() const;
    void setDistanceUnit(DistanceUnit unit);

    QDate sessionDate() const;
    void setSessionDate(const QDate &date);

    // ===== Impacts =====
    int impactCount() const;
    QList<ShotImpact> impacts() const;
    /// Returns the impact at the given index, or an empty ShotImpact if index is invalid
    ShotImpact impactAt(int index) const;
    /// Adds a new impact and invalidates cached statistics
    void addImpact(const ShotImpact &impact);
    /// Removes the impact at the given index if valid, and invalidates cached statistics
    void removeImpact(int index);
    /// Removes all impacts and invalidates cached statistics
    void clearImpacts();
    /// Replaces all impacts with the provided list and invalidates cached statistics
    void replaceImpacts(const QList<ShotImpact> &impacts);
    /// Returns the next available unique impact ID and increments the counter
    int nextImpactId();

    // ===== Metadata =====
    QString firearm() const;
    void setFirearm(const QString &firearm);

    QString ammunition() const;
    void setAmmunition(const QString &ammo);

    QString notes() const;
    void setNotes(const QString &notes);

    // ===== Visualization Settings =====
    bool showFullGroupCircle() const;
    void setShowFullGroupCircle(bool show);

    bool show80PercentCircle() const;
    void setShow80PercentCircle(bool show);

    bool show90PercentCircle() const;
    void setShow90PercentCircle(bool show);

    bool showPointOfAim() const;
    void setShowPointOfAim(bool show);

    PlaqueConfig plaqueConfig() const;
    void setPlaqueConfig(const PlaqueConfig &config);

    // ===== Statistics =====
    /// @brief Returns the current statistics
    /// 
    /// Statistics are automatically updated when impacts or scale factor change.
    /// The returned reference remains valid until the document is destroyed.
    /// 
    /// \return Reference to the Statistics object
    const Statistics& statistics() const;

    // ===== File Management =====
    QString filePath() const;
    void setFilePath(const QString &path);
    bool hasFilePath() const;

    bool isDirty() const;
    void setDirty(bool dirty);

    // ===== Workflow State Queries =====
    /// Returns true if caliber is set (enables workflow step 2: scale factor)
    bool canEnableScaleFactorState() const;
    /// Returns true if scale factor is set (enables workflow step 3: point of aim)
    bool canEnablePointOfAimState() const;
    /// Returns true if scale factor is set (enables workflow step 4: mark impacts)
    bool canEnableMarkImpactsState() const;
    /// Returns true if at least one impact exists (enables workflow step 5: visualization)
    bool canEnableVisualizationState() const;

    // ===== Serialization =====
    /// @brief Saves the document to a file
    /// 
    /// \param filePath The destination file path
    /// \param errorMsg Optional pointer to receive error message on failure
    /// \return True if save succeeded, false otherwise
    /// \note Currently not implemented (returns false)
    bool saveToFile(const QString &filePath, QString *errorMsg = nullptr);
    /// @brief Loads the document from a file
    /// 
    /// \param filePath The source file path
    /// \param errorMsg Optional pointer to receive error message on failure
    /// \return True if load succeeded, false otherwise
    /// \note Currently not implemented (returns false)
    bool loadFromFile(const QString &filePath, QString *errorMsg = nullptr);

    /// @brief Serializes the document to a JSON object
    /// 
    /// Includes all document data: image dimensions, calibration, impacts,
    /// metadata, and visualization settings. The image data itself is not
    /// included in the JSON (use saveToFile for complete serialization).
    /// 
    /// \return JSON object containing document data
    QJsonObject toJson() const;
    /// @brief Deserializes the document from a JSON object
    /// 
    /// Loads all document data from JSON. The target image must be set
    /// separately (it's not stored in JSON). Statistics are automatically
    /// invalidated after loading.
    /// 
    /// \param json The JSON object to load from
    /// \param errorMsg Optional pointer to receive error message on failure
    /// \return True if deserialization succeeded, false otherwise
    bool fromJson(const QJsonObject &json, QString *errorMsg = nullptr);

signals:
    /// Emitted when any document data changes (image, calibration, impacts, metadata)
    void dataChanged();
    /// Emitted when the dirty state changes (unsaved changes flag)
    void dirtyChanged(bool dirty);
    /// Emitted when impacts are added, removed, or replaced
    void impactsChanged();
    /// Emitted when statistics values have changed
    void statisticsChanged();
    /// Emitted when visualization settings change (circles, plaque, etc.)
    void visualizationSettingsChanged();

private:
    // Image
    QImage m_targetImage;
    
    // Calibration
    double m_bulletDiameter = 0.0;
    double m_pixelsPerInch = 0.0;
    
    // Point of Aim
    QPointF m_pointOfAim;
    bool m_hasPointOfAim = false;
    
    // Session parameters
    double m_targetDistance = 100.0;
    DistanceUnit m_distanceUnit = DistanceUnit::Yards;
    QDate m_sessionDate;
    
    // Impacts
    QList<ShotImpact> m_impacts;
    int m_nextImpactId = 1;
    
    // Metadata
    QString m_firearm;
    QString m_ammunition;
    QString m_notes;
    
    // Visualization settings
    bool m_showFullGroupCircle = true;
    bool m_show80PercentCircle = false;
    bool m_show90PercentCircle = false;
    bool m_showPointOfAim = true;
    PlaqueConfig m_plaqueConfig;
    
    // Statistics
    Statistics m_statistics;
    
    // File management
    QString m_filePath;
    bool m_isDirty = false;
    
    // Helper methods
    void updateStatistics();
};
