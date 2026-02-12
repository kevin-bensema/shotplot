#pragma once

#include <QGraphicsView>
#include <QPointF>

class WorkflowState;
class TargetScene;

/// @brief Custom QGraphicsView for target image display and interaction
///
/// TargetView provides a specialized view widget for displaying and interacting
/// with target images in the ShotPlot application. It extends QGraphicsView with
/// custom zoom and pan capabilities, and delegates state-specific mouse interactions
/// to the current WorkflowState.
///
/// Key features:
/// - Mouse wheel zooming with configurable zoom limits
/// - Panning via middle mouse button or Ctrl+left drag
/// - Click vs drag detection (distinguishes clicks from pan gestures)
/// - State-aware cursor management
/// - Mouse position tracking for status bar display
/// - Delegation of click/move events to WorkflowState for state-specific behavior
///
/// The view is configured with smooth rendering, no scroll bars (manual pan/zoom),
/// and mouse tracking enabled. Mouse events are routed to the current WorkflowState
/// for handling state-specific interactions such as marking impacts or calibrating scale.
class TargetView : public QGraphicsView
{
    Q_OBJECT

public:
    /// Constructs a TargetView with the given parent widget
    ///
    /// Initializes the view with smooth rendering, disabled scroll bars,
    /// mouse tracking enabled, and a dark gray background. The view is
    /// configured to use manual pan/zoom handling rather than Qt's default
    /// drag mode.
    /// \param pParent The parent widget
    explicit TargetView(QWidget* pParent = nullptr);

    /// Sets the target scene for this view
    ///
    /// Stores a typed TargetScene pointer and calls the inherited
    /// QGraphicsView::setScene() to connect the view to the scene.
    /// This provides typed access to TargetScene's API without needing
    /// to search through scene items or downcast.
    /// \param pScene The target scene to display, or nullptr to disconnect
    void setTargetScene(TargetScene* pScene);
    
    /// Returns the typed target scene
    /// \return The target scene, or nullptr if none is set
    TargetScene* targetScene() const;

    /// Sets the current workflow state for event delegation
    ///
    /// When a state is set, mouse events (clicks, moves) are delegated to
    /// the state's handler methods. The cursor is updated to match the
    /// state's preferred cursor. Setting nullptr disables state delegation
    /// and resets the cursor to the default arrow.
    /// \param pState The workflow state to receive mouse events, or nullptr to disable
    void setWorkflowState(WorkflowState* pState);
    
    /// Returns the current workflow state
    /// \return The current workflow state, or nullptr if none is set
    WorkflowState* workflowState() const;

    // Zoom controls
    
    /// Zooms in by the configured zoom step factor
    ///
    /// Increases the zoom level by ZOOM_STEP (1.15x) if the new zoom
    /// factor would not exceed MAX_ZOOM. The zoom is centered on the
    /// current mouse position.
    void zoomIn();
    
    /// Zooms out by the configured zoom step factor
    ///
    /// Decreases the zoom level by dividing by ZOOM_STEP (1.15x) if the
    /// new zoom factor would not fall below MIN_ZOOM. The zoom is centered
    /// on the current mouse position.
    void zoomOut();
    
    /// Fits the entire scene within the view while maintaining aspect ratio
    ///
    /// Resets the transform and calculates a zoom factor that fits the
    /// scene's bounding rectangle in the viewport. Updates the internal
    /// zoom factor tracking to match the actual transform. Does nothing
    /// if no scene is set or the scene rectangle is empty.
    void zoomFit();

signals:
    /// Emitted when the mouse position changes over the view
    ///
    /// This signal is emitted continuously during mouse movement to provide
    /// real-time position updates for status bar display or other UI feedback.
    /// The position is in scene coordinates (pixel coordinates relative to
    /// the target image).
    /// \param scenePos The current mouse position in scene coordinates
    void mousePositionChanged(const QPointF& scenePos);

protected:
    /// Handles mouse wheel events for zooming
    ///
    /// Scrolls up zoom in, scrolls down zoom out. The zoom is applied
    /// immediately and respects the configured zoom limits.
    /// \param pEvent The wheel event containing scroll delta information
    void wheelEvent(QWheelEvent* pEvent) override;
    
    /// Handles mouse button press events
    ///
    /// Initiates panning for middle button or Ctrl+left button. Records
    /// the press position for click vs drag detection. Right clicks are
    /// immediately delegated to the current state's handleRightClick().
    /// Left clicks are deferred to mouseReleaseEvent to distinguish clicks
    /// from drag gestures.
    /// \param pEvent The mouse press event
    void mousePressEvent(QMouseEvent* pEvent) override;
    
    /// Handles mouse movement events
    ///
    /// Emits mousePositionChanged signal for status bar updates. If panning
    /// is active, updates the scroll bar positions. Otherwise, checks if
    /// a left-button drag exceeds CLICK_THRESHOLD to initiate panning, and
    /// delegates movement to the current state's handleMouseMove() for
    /// state-specific visual feedback.
    /// \param pEvent The mouse move event
    void mouseMoveEvent(QMouseEvent* pEvent) override;
    
    /// Handles mouse button release events
    ///
    /// Ends panning mode if active. For left button releases, checks if
    /// the movement distance was below CLICK_THRESHOLD to determine if
    /// this was a click (not a drag), and if so, delegates to the current
    /// state's handleMouseClick().
    /// \param pEvent The mouse release event
    void mouseReleaseEvent(QMouseEvent* pEvent) override;

    /// Overrides paintEvent to draw the custom cursor glyph if needed
    void paintEvent(QPaintEvent* pEvent) override;

    /// Overrides enterEvent to track when the mouse enters the view
    void enterEvent(QEnterEvent* pEvent) override;

    /// Overrides leaveEvent to track when the mouse leaves the view
    void leaveEvent(QEvent* pEvent) override;

private:
    /// Updates the cursor based on the current workflow state
    ///
    /// Sets the cursor to the state's preferred cursor if a state is active,
    /// otherwise resets to the default arrow cursor. Called when the state
    /// changes or when panning ends.
    void updateCursor();
    
    /// Updates the background color from the graphics settings service
    void updateBackgroundColor();

    /// Returns true if a QGraphicsItem has grabbed the mouse (e.g. during drag/resize)
    bool isItemGrabbingMouse() const;

private:
    /// The current workflow state that receives delegated mouse events
    ///
    /// When set, this state handles clicks, right-clicks, and mouse movement
    /// for state-specific behavior. The cursor is synchronized with the state's
    /// preferred cursor.
    WorkflowState* m_pCurrentState = nullptr;
    
    /// The typed target scene for direct access to scene items
    TargetScene* m_pTargetScene = nullptr;
    
    // Pan/zoom state
    
    /// Last recorded mouse position for calculating pan deltas
    QPointF m_lastMousePos;
    
    /// Mouse position at button press for click vs drag detection
    QPointF m_mousePressPos;
    
    /// Whether the view is currently being panned
    ///
    /// Set to true when middle button or Ctrl+left drag is active, or when
    /// a left drag exceeds CLICK_THRESHOLD. When true, mouse movement pans
    /// the view instead of being delegated to the workflow state.
    bool m_isPanning = false;

    /// Whether the mouse is currently over the view
    bool m_mouseInView = false;

    /// Current mouse position in viewport coordinates
    QPoint m_currentMousePos;
};
