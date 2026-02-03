#pragma once

#include "WorkflowState.h"

class TargetView;
class TargetScene;

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
    /// Constructs a POAState with the given document, view, and scene.
    /// \param document The shot group document to modify
    /// \param view The target view widget
    /// \param scene The graphics scene containing target elements
    POAState(ShotGroupDocument *document, TargetView *view, TargetScene *scene);
    
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
    void handleMouseClick(const QPointF &scenePos) override;
    
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
    /// \param toolbar The toolbar widget to populate
    void populateToolbar(QToolBar *toolbar) override;
    
    /// Returns the display name of this state.
    /// \return "Point of Aim"
    QString stateName() const override
    {
        return tr("Point of Aim");
    }

private:
    TargetView *m_view;      ///< The target view widget for interaction
    TargetScene *m_scene;    ///< The graphics scene for displaying POA glyph
};
