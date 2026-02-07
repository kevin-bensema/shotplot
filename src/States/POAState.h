#pragma once

#include "WorkflowState.h"

class TargetView;

/**
 * @brief State for marking the Point of Aim
 * 
 * This is the third state in the ShotPlot workflow, following scale calibration.
 * It allows users to mark their intended point of impact on the target image.
 * The Point of Aim (POA) is optional - users can skip this step if desired.
 * 
 * When a user clicks on the target, the POA is set at that location and displayed
 * as an X-shaped glyph. The state automatically transitions to the next state
 * after marking. If a POA was previously set, it is displayed when entering
 * this state.
 * 
 * The POA glyph remains visible after exiting this state and can be used for
 * future adjustment calculations.
 */
class POAState : public WorkflowState
{
    Q_OBJECT

public:
    /// Constructs a POAState with the given document and view.
    /// \param pDocument The shot group document to modify
    /// \param pView The target view widget (provides access to the target scene)
    POAState(ShotGroupDocument* pDocument, TargetView* pView);
    
    ~POAState();

    /// Called when entering this state.
    /// Displays any existing POA glyph if one was previously set.
    void onEnter() override;
    
    /// Called when exiting this state.
    /// The POA glyph remains visible after state exit.
    void onExit() override;
    
    /// Handles a mouse click at the given scene position.
    /// Sets the Point of Aim at the clicked location, updates the scene glyph,
    /// and automatically transitions to the next state.
    /// \param scenePos The scene coordinates where the user clicked
    void handleMouseClick(const QPointF& scenePos) override;
    
    /// Checks if this state is complete.
    /// POA is optional, so completion is determined by whether the scale factor
    /// has been set (required for subsequent states).
    /// \return True if the scale factor is set, false otherwise
    bool isComplete() const override;
    
    /// Returns the cursor to display in this state.
    /// Uses a cross cursor to indicate point selection.
    /// \return A cross cursor (Qt::CrossCursor)
    QCursor cursor() const override;
    
    /// Populates the per-state toolbar with controls for this state.
    /// Adds instructions and a "Skip" button to allow users to bypass POA marking.
    /// \param pToolbar The toolbar widget to populate
    void populateToolbar(QToolBar* pToolbar) override;
    
    /// Returns the display name of this state.
    /// \return "Point of Aim"
    QString stateName() const override;

    /// Draws a segmented circle cursor at the mouse position.
    /// \param pPainter The painter to use for drawing on the viewport
    /// \param viewportPos The current mouse position in viewport coordinates
    /// \param zoomFactor The current zoom factor of the view
    void drawCursor(QPainter* pPainter, const QPointF& viewportPos, double zoomFactor) override;

private:
    TargetView* m_pView;      ///< The target view widget (provides access to the target scene)
};
