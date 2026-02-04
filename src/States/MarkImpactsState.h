#pragma once

#include "WorkflowState.h"

class TargetView;
class TargetScene;
class QUndoStack;

/// @brief Main working state for marking shot impacts on the target
/// 
/// This state allows users to interactively mark shot impacts by clicking
/// on the target image. Left-click adds a new impact at the clicked position,
/// and right-click removes the nearest impact. The state automatically updates
/// group circles (full, 80%, 90%) as impacts are added or removed.
/// 
/// All impact modifications are performed through undo/redo commands, ensuring
/// that changes can be reversed. The state maintains synchronization between
/// the document's impact data and the visual glyphs displayed in the scene.
class MarkImpactsState : public WorkflowState
{
    Q_OBJECT

public:
    /// Constructs a new MarkImpactsState
    /// \param pDocument The document containing impact data
    /// \param pView The target view widget
    /// \param pScene The graphics scene for rendering impact glyphs
    /// \param pUndoStack The undo stack for command-based modifications
    MarkImpactsState(ShotGroupDocument* pDocument, TargetView* pView, 
                     TargetScene* pScene, QUndoStack* pUndoStack);
    ~MarkImpactsState();

    /// Initializes the state by recreating impact glyphs from document data
    /// and connecting to document change signals for automatic circle updates.
    void onEnter() override;
    
    /// Cleans up signal connections when leaving the state.
    void onExit() override;
    
    /// Adds a new impact at the specified scene position.
    /// \param scenePos The position in scene coordinates where the impact occurred
    void handleMouseClick(const QPointF& scenePos) override;
    
    /// Removes the impact nearest to the specified scene position if within hit radius.
    /// \param scenePos The position in scene coordinates to check for impact removal
    void handleRightClick(const QPointF& scenePos) override;
    
    /// Returns true if at least one impact has been marked.
    bool isComplete() const override;
    
    /// Returns a crosshair cursor for precise impact marking.
    QCursor cursor() const override;
    
    /// Populates the toolbar with state-specific controls, including a "Clear All" button.
    /// \param pToolbar The toolbar widget to populate
    void populateToolbar(QToolBar* pToolbar) override;
    
    /// Returns the display name of this state.
    QString stateName() const override;

private:
    /// Updates group circle visualizations based on current impact statistics.
    void updateGroupCircles();
    /// Calculates the bullet diameter in pixels based on document scale factor.
    /// Returns a default value if scale factor is not yet set.
    /// \return The bullet diameter in pixels, or 20.0 as fallback
    double bulletDiameterPixels() const;

private:
    TargetView* m_pView;      ///< The target view widget for display
    TargetScene* m_pScene;    ///< The graphics scene for rendering impact glyphs and circles
    QUndoStack* m_pUndoStack; ///< The undo stack for command-based modifications
};
