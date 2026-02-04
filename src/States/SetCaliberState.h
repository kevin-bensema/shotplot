#pragma once

#include "WorkflowState.h"

class TargetView;

/// @brief Workflow state for setting bullet diameter (caliber)
///
/// This is the first state in the ShotPlot workflow. It presents a modal dialog
/// (CaliberDialog) to the user for selecting the bullet diameter, which is required
/// for accurate shot marking and group size calculations. The diameter is stored
/// in the document and used throughout the analysis workflow.
///
/// Unlike other workflow states that use mouse interactions, this state relies
/// entirely on a modal dialog interface. Mouse clicks are ignored during this state.
///
/// The state is considered complete when the document has a caliber value set.
/// Upon successful dialog acceptance, the state emits stateCompleted() and
/// requestNextState() signals to transition to the next workflow step.
class SetCaliberState : public WorkflowState
{
    Q_OBJECT

public:
    /// @brief Constructs a new SetCaliberState
    ///
    /// \param pDocument The shot group document to store the caliber value
    /// \param pView The target view (currently unused, reserved for future use)
    /// \param pParentWidget Parent widget for the caliber dialog (typically the main window)
    SetCaliberState(ShotGroupDocument* pDocument, TargetView* pView, QWidget* pParentWidget);
    
    ~SetCaliberState();

    /// @brief Called when entering this state
    ///
    /// Displays a CaliberDialog modal dialog. If the document already has a caliber
    /// set, the dialog is pre-populated with that value. If the user accepts the
    /// dialog, the bullet diameter is saved to the document and signals are emitted
    /// to complete the state and request transition to the next state.
    void onEnter() override;
    
    /// @brief Called when exiting this state
    ///
    /// Currently performs no cleanup operations.
    void onExit() override;
    
    /// @brief Handles mouse click events
    ///
    /// This state uses a modal dialog interface rather than mouse interactions,
    /// so mouse clicks are ignored. This method is overridden to satisfy the
    /// base class interface but performs no action.
    ///
    /// \param scenePos The scene position of the mouse click (unused)
    void handleMouseClick(const QPointF& scenePos) override;
    
    /// @brief Checks if this state has been completed
    ///
    /// The state is complete when the document has a bullet diameter (caliber)
    /// value set.
    ///
    /// \return True if the document has a caliber set, false otherwise
    bool isComplete() const override;
    
    /// @brief Populates the per-state toolbar with state-specific controls
    ///
    /// Adds an instructional label informing the user to select the bullet
    /// diameter from the dialog.
    ///
    /// \param pToolbar The toolbar widget to populate
    void populateToolbar(QToolBar* pToolbar) override;
    
    /// @brief Returns the display name of this state
    ///
    /// \return Localized string "Set Caliber"
    QString stateName() const override;

private:
    TargetView* m_pView;          ///< The target view (reserved for future use)
    QWidget* m_pParentWidget;     ///< Parent widget for the caliber dialog
};
