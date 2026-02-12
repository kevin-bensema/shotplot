#pragma once

#include <QObject>
#include <QCursor>
#include <QPointF>

class QToolBar;
class QPainter;
class ShotGroupDocument;
class TargetView;
class TargetScene;

/// @brief Abstract base class for workflow states in the ShotPlot application
///
/// WorkflowState implements the State pattern to manage the different stages
/// of the shot group analysis workflow. Each concrete state (SetCaliberState,
/// ScaleFactorState, POAState, MarkImpactsState, VisualizationState) handles
/// a specific phase of the user's interaction with the target image.
///
/// The workflow state system coordinates user interactions, visual feedback,
/// and document modifications. States are managed by MainWindow, which calls
/// onEnter() when transitioning into a state and onExit() when leaving it.
/// Mouse events from the target view are routed to the current state's
/// handleMouseClick(), handleRightClick(), and handleMouseMove() methods.
///
/// Each state can populate a per-state toolbar with controls specific to
/// that workflow phase. States signal completion through stateCompleted()
/// or requestNextState() to trigger automatic workflow progression.
class WorkflowState : public QObject
{
    Q_OBJECT

public:
    /// Constructs a workflow state with the given document
    /// \param document The shot group document that this state will operate on
    /// \param parent The parent QObject for Qt's object hierarchy
    explicit WorkflowState(ShotGroupDocument* pDocument, QObject* pParent = nullptr);
    
    virtual ~WorkflowState();

    /// Called when entering this state during workflow transitions
    ///
    /// Implementations should perform initialization such as setting up
    /// visual feedback, restoring previous state, or configuring UI elements.
    /// This is called by MainWindow when transitioning from another state.
    virtual void onEnter() = 0;
    
    /// Called when exiting this state during workflow transitions
    ///
    /// Implementations should perform cleanup such as hiding temporary
    /// visual elements or disconnecting signal handlers. This is called
    /// by MainWindow before transitioning to another state.
    virtual void onExit() = 0;
    
    /// Handles left mouse button clicks at the specified scene position
    ///
    /// This is the primary interaction method for most states. Implementations
    /// should perform state-specific actions such as marking points, adding
    /// impacts, or triggering state transitions. The position is in scene
    /// coordinates (pixel coordinates relative to the target image).
    /// \param scenePos The mouse click position in scene coordinates
    virtual void handleMouseClick(const QPointF& scenePos) = 0;
    
    /// Handles right mouse button clicks at the specified scene position
    ///
    /// Default implementation does nothing. Override in derived classes to
    /// provide right-click functionality such as removing impacts or canceling
    /// operations. The position is in scene coordinates.
    /// \param scenePos The mouse click position in scene coordinates
    virtual void handleRightClick(const QPointF& scenePos);
    
    /// Handles mouse movement at the specified scene position
    ///
    /// Default implementation does nothing. Override in derived classes to
    /// provide real-time visual feedback such as preview lines or cursor
    /// updates during mouse movement. The position is in scene coordinates.
    /// \param scenePos The current mouse position in scene coordinates
    virtual void handleMouseMove(const QPointF& scenePos);
    
    /// Checks whether this state has been completed and can transition
    ///
    /// The completion criteria are state-specific. For example, SetCaliberState
    /// is complete when a caliber has been selected, while MarkImpactsState
    /// is complete when at least one impact has been marked. This is used
    /// by the workflow toolbar to enable/disable state progression.
    /// \return True if the state's required actions have been completed
    virtual bool isComplete() const = 0;
    
    /// Returns the cursor to display when this state is active
    ///
    /// Default implementation returns Qt::ArrowCursor. Override in derived
    /// classes to provide state-specific cursors such as crosshairs for
    /// precise point selection or custom cursors for impact marking.
    /// \return The cursor to display for this state
    virtual QCursor cursor() const;
    
    /// Populates the per-state toolbar with controls specific to this workflow phase
    ///
    /// Each state can add buttons, labels, spinboxes, or other widgets to
    /// the toolbar that appears below the main workflow toolbar. The toolbar
    /// is cleared before this method is called, so implementations should
    /// add all necessary controls. This is called by MainWindow when entering
    /// the state.
    /// \param toolbar The toolbar widget to populate with state-specific controls
    virtual void populateToolbar(QToolBar* pToolbar) = 0;
    
    /// Returns the display name for this state
    ///
    /// Used in the workflow toolbar and UI to identify the current state.
    /// Should return a localized, user-friendly name.
    /// \return The state's display name (e.g., "Scale Factor", "Mark Impacts")
    virtual QString stateName() const = 0;

    /// Symbolic constants for workflow state indices
    ///
    /// These constants define the standard ordering of states in the workflow.
    /// Use these instead of hardcoded integer indices to improve readability
    /// and maintainability of state transition logic.
    enum StateIndex {
        kSetCaliberStateIndex = 0,
        kScaleFactorStateIndex = 1,
        kPOAStateIndex = 2,
        kMarkImpactsStateIndex = 3,
        kVisualizationStateIndex = 4,
        kStateCount = 5
    };

    /// Draws a custom cursor glyph if this state requires one
    ///
    /// Called by TargetView during paintEvent to allow the state to draw a custom
    /// cursor overlay on the viewport. The default implementation does nothing.
    /// States that need custom cursors (e.g., segmented circles for impact marking)
    /// should override this method.
    /// \param pPainter The painter to use for drawing on the viewport
    /// \param viewportPos The current mouse position in viewport coordinates
    /// \param zoomFactor The current zoom factor of the view (1.0 = 100%)
    virtual void drawCursor(QPainter* pPainter, const QPointF& viewportPos, double zoomFactor);

signals:
    /// Emitted when the state has completed its required actions
    ///
    /// Derived classes should emit this signal when the state's primary
    /// objective has been achieved. This can be used to trigger automatic
    /// workflow progression or update UI indicators.
    void stateCompleted();
    
    /// Emitted to request transition to the next workflow state
    ///
    /// Derived classes should emit this signal when they want to advance
    /// to the next state in the workflow sequence. MainWindow listens to
    /// this signal to perform the state transition.
    void requestNextState();

protected:
    /// The shot group document that this state operates on
    ///
    /// All states have access to the document for reading and modifying
    /// calibration data, impacts, metadata, and other session information.
    /// This pointer is set during construction and remains valid for the
    /// lifetime of the state object.
    ShotGroupDocument* m_pDocument;
};
