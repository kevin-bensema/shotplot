#pragma once

#include <QToolBar>

class WorkflowState;

/// @brief Toolbar that displays state-specific controls for the current workflow state.
///
/// PerStateToolbar is a dynamic toolbar that adapts its contents based on the current
/// WorkflowState. When a state is set via setCurrentState(), the toolbar is cleared
/// and repopulated with controls specific to that state. Each WorkflowState implementation
/// defines its own toolbar controls through the populateToolbar() method.
///
/// The toolbar is non-movable and displays the title "State Controls" to indicate
/// its purpose in the application workflow.
class PerStateToolbar : public QToolBar
{
    Q_OBJECT

public:
    /// Constructs a PerStateToolbar with the given parent.
    ///
    /// Initializes the toolbar as non-movable with the title "State Controls".
    /// \param parent The parent widget, or nullptr if the toolbar has no parent.
    explicit PerStateToolbar(QWidget* pParent = nullptr);
    
    /// Destructor.
    ~PerStateToolbar();

    /// Sets the current workflow state and updates the toolbar accordingly.
    ///
    /// Clears all existing toolbar actions and widgets, then calls populateToolbar()
    /// on the new state (if not nullptr) to populate the toolbar with state-specific
    /// controls. The toolbar will be empty if state is nullptr.
    ///
    /// \param state The WorkflowState to display controls for, or nullptr to clear the toolbar.
    void setCurrentState(WorkflowState* pState);

private:
    /// Clears all actions and widgets from the toolbar.
    void clearToolbar();

private:
    /// Pointer to the currently active WorkflowState, or nullptr if no state is set.
    WorkflowState* m_pCurrentState = nullptr;
};
