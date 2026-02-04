#pragma once

#include <QWidget>

class QLabel;
class QCheckBox;
class ShotGroupDocument;

/// @brief Panel displaying shot group statistics and display options
///
/// StatisticsPanel provides a widget that displays comprehensive statistics
/// for a shot group, including shot count, group sizes (full, 80%, 90%),
/// mean radius, and standard deviation. All measurements are displayed in
/// the user's preferred unit system (inches, MOA, or mrad) and are automatically
/// converted based on the document's calibration data.
///
/// The panel also provides checkboxes to toggle visualization options:
/// - Full Group Circle: Shows/hides the full group boundary circle
/// - Point of Aim: Shows/hides the point of aim marker
///
/// Statistics are automatically updated when the document's impacts change
/// or when statistics are invalidated. The panel displays "--" for statistics
/// that cannot be calculated (requires at least 2 impacts and a valid scale factor).
class StatisticsPanel : public QWidget
{
    Q_OBJECT

public:
    /// Constructs a statistics panel widget
    /// \param pParent The parent widget
    explicit StatisticsPanel(QWidget* pParent = nullptr);
    
    ~StatisticsPanel();

    /// Sets the document to display statistics for
    ///
    /// Connects to the document's signals to automatically update statistics
    /// when impacts change or statistics are invalidated. Also synchronizes
    /// the display option checkboxes with the document's current visualization
    /// settings.
    /// \param pDocument The shot group document to display statistics for (can be nullptr)
    void setDocument(ShotGroupDocument* pDocument);
    
    /// Manually refreshes the statistics display
    ///
    /// Forces an immediate update of all displayed statistics. This is typically
    /// called automatically when the document changes, but can be used to
    /// refresh the display after external changes.
    void updateDisplay();

private:
    void setupUi();
    void updateStatistics();

private:
    /// The shot group document providing statistics data
    ///
    /// Must be set via setDocument() before statistics can be displayed.
    /// The panel automatically updates when this document's impacts change
    /// or when statistics are invalidated.
    ShotGroupDocument* m_pDocument = nullptr;
    
    QLabel* m_pShotCountLabel = nullptr;
    QLabel* m_pFullGroupLabel = nullptr;
    QLabel* m_pGroup80Label = nullptr;
    QLabel* m_pGroup90Label = nullptr;
    QLabel* m_pMeanRadiusLabel = nullptr;
    QLabel* m_pStdDevLabel = nullptr;
    
    QCheckBox* m_pShowFullGroupCheck = nullptr;
    QCheckBox* m_pShowPOACheck = nullptr;
};

