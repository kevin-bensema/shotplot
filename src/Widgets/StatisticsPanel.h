#ifndef STATISTICSPANEL_H
#define STATISTICSPANEL_H

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
    /// \param parent The parent widget
    explicit StatisticsPanel(QWidget *parent = nullptr);
    
    ~StatisticsPanel();

    /// Sets the document to display statistics for
    ///
    /// Connects to the document's signals to automatically update statistics
    /// when impacts change or statistics are invalidated. Also synchronizes
    /// the display option checkboxes with the document's current visualization
    /// settings.
    /// \param doc The shot group document to display statistics for (can be nullptr)
    void setDocument(ShotGroupDocument *doc);
    
    /// Manually refreshes the statistics display
    ///
    /// Forces an immediate update of all displayed statistics. This is typically
    /// called automatically when the document changes, but can be used to
    /// refresh the display after external changes.
    void updateDisplay();

private slots:
    /// Handles changes to the "Full Group Circle" checkbox
    ///
    /// Updates the document's visualization setting to show or hide
    /// the full group boundary circle based on the checkbox state.
    /// \param checked True to show the full group circle, false to hide it
    void onShowFullGroupChanged(bool checked);
    
    /// Handles changes to the "Point of Aim" checkbox
    ///
    /// Updates the document's visualization setting to show or hide
    /// the point of aim marker based on the checkbox state.
    /// \param checked True to show the point of aim, false to hide it
    void onShowPOAChanged(bool checked);

private:
    void setupUi();
    void updateStatistics();

private:
    /// The shot group document providing statistics data
    ///
    /// Must be set via setDocument() before statistics can be displayed.
    /// The panel automatically updates when this document's impacts change
    /// or when statistics are invalidated.
    ShotGroupDocument *m_document = nullptr;
    
    QLabel *m_shotCountLabel = nullptr;
    QLabel *m_fullGroupLabel = nullptr;
    QLabel *m_group80Label = nullptr;
    QLabel *m_group90Label = nullptr;
    QLabel *m_meanRadiusLabel = nullptr;
    QLabel *m_stdDevLabel = nullptr;
    
    QCheckBox *m_showFullGroupCheck = nullptr;
    QCheckBox *m_showPOACheck = nullptr;
};

#endif // STATISTICSPANEL_H
