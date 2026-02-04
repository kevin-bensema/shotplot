#pragma once

#include "WorkflowState.h"

class TargetView;
class TargetScene;

/// @brief State for configuring visualization and export
///
/// This is the final state in the ShotPlot workflow, allowing users to configure
/// visualization options and export annotated images. The state becomes active
/// after shot impacts have been marked and enables customization of statistics
/// plaques, group circle overlays, and export settings.
///
/// The state requires at least one impact to be marked before it can be considered
/// complete. Future implementation will support interactive placement and dragging
/// of statistics plaques on the target visualization.
class VisualizationState : public WorkflowState
{
    Q_OBJECT

public:
    /// Constructs a VisualizationState with the given document, view, and scene
    ///
    /// \param document The shot group document containing impacts and calibration data
    /// \param view The target view widget for displaying the visualization
    /// \param scene The graphics scene containing target graphics items
    VisualizationState(ShotGroupDocument* pDocument, TargetView* pView, TargetScene* pScene);
    
    ~VisualizationState();

    /// Initializes visualization configuration UI when entering this state
    ///
    /// Sets up the visualization interface and prepares the scene for
    /// plaque placement and export configuration.
    void onEnter() override;
    
    /// Cleans up visualization state when exiting
    ///
    /// Performs any necessary cleanup when transitioning away from this state.
    void onExit() override;
    
    /// Handles mouse clicks for plaque placement and dragging
    ///
    /// Future implementation will allow users to click and drag statistics
    /// plaques to position them on the target visualization.
    /// \param scenePos The scene coordinates where the click occurred
    void handleMouseClick(const QPointF& scenePos) override;
    
    /// Checks if the visualization state is complete
    ///
    /// The state is complete when the document exists and contains at least
    /// one impact. This ensures there is data to visualize before allowing
    /// export or progression.
    /// \return True if the document has at least one impact, false otherwise
    bool isComplete() const override;
    
    /// Populates the toolbar with visualization configuration controls
    ///
    /// Adds widgets to the per-state toolbar for configuring visualization
    /// options such as statistics plaque settings, group circle visibility,
    /// and export parameters.
    /// \param toolbar The toolbar to populate with visualization controls
    void populateToolbar(QToolBar* pToolbar) override;
    
    QString stateName() const override;

private:
    /// The target view widget displaying the visualization
    TargetView* m_pView;
    
    /// The graphics scene containing target graphics items and overlays
    TargetScene* m_pScene;
};
