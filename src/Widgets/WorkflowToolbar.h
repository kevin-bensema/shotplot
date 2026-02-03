#ifndef WORKFLOWTOOLBAR_H
#define WORKFLOWTOOLBAR_H

#include <QToolBar>
#include <QList>

class QToolButton;
class ShotGroupDocument;

/// @brief Toolbar displaying the workflow pipeline states
/// 
/// Provides a visual representation of the shot group analysis workflow pipeline
/// with buttons for each state: Caliber, Scale, POA, Mark Impacts, and Visualization.
/// Buttons are connected with arrows (→) to show the sequential workflow progression.
/// 
/// The toolbar automatically enables/disables state buttons based on the document's
/// current state and completion status. Only states that can be accessed based on
/// prerequisite completion are enabled. When a button is clicked, the stateSelected
/// signal is emitted to notify the application to switch to that workflow state.
/// 
/// The toolbar is non-movable and connects to the document's dataChanged signal
/// to automatically update button enablement when the document state changes.
class WorkflowToolbar : public QToolBar
{
    Q_OBJECT

public:
    explicit WorkflowToolbar(QWidget *parent = nullptr);
    ~WorkflowToolbar();

    /// Associates a document with this toolbar and updates button enablement.
    ///
    /// Connects to the document's dataChanged signal to automatically update
    /// button states when the document changes. If nullptr is passed, all
    /// buttons are disabled.
    /// \param doc The document to associate with this toolbar, or nullptr to clear.
    void setDocument(ShotGroupDocument *doc);
    
    /// Sets the currently active workflow state and updates button visual state.
    ///
    /// Updates the checked state of buttons to reflect which workflow state
    /// is currently active. The button at the specified index will be checked,
    /// and all others will be unchecked.
    /// \param index The zero-based index of the state to mark as current.
    ///              Use -1 to clear the current state selection.
    void setCurrentState(int index);
    
    /// Updates the enabled/disabled state of all workflow buttons.
    ///
    /// Evaluates the document's current state and enables only those workflow
    /// states that can be accessed based on prerequisite completion:
    /// - Caliber: Always enabled when document exists
    /// - Scale: Enabled after caliber is set
    /// - POA: Enabled after scale factor is set
    /// - Mark Impacts: Enabled after scale factor is set
    /// - Visualization: Enabled after at least one impact is marked
    ///
    /// This is called automatically when the document changes or emits dataChanged.
    void updateStateEnablement();

signals:
    /// Emitted when a workflow state button is clicked.
    ///
    /// \param index The zero-based index of the selected workflow state.
    void stateSelected(int index);

private slots:
    void onButtonClicked();

private:
    void setupButtons();

private:
    ShotGroupDocument *m_document = nullptr;
    QList<QToolButton*> m_buttons;
    int m_currentState = -1;

    static const QStringList STATE_NAMES;
};

#endif // WORKFLOWTOOLBAR_H
