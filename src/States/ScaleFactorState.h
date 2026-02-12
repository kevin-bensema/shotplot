#pragma once

#include "WorkflowState.h"

class TargetView;
class QDoubleSpinBox;

/// @brief State for interactive scale calibration
///
/// This state allows users to calibrate the scale factor by clicking two points
/// on the target image and entering the real-world distance between them. The
/// calibration process:
/// 1. User clicks the first point (start of reference line)
/// 2. User moves mouse to preview the line
/// 3. User clicks the second point (end of reference line)
/// 4. User enters the actual distance in inches via the toolbar spinbox
/// 5. The scale factor (pixels per inch) is calculated and stored in the document
///
/// The state provides visual feedback by displaying a green scale line between
/// the two points during the interaction. Once calibration is complete, the
/// scale factor is used to convert between pixel coordinates and physical units
/// (inches, MOA, mrad) throughout the application.
class ScaleFactorState : public WorkflowState
{
    Q_OBJECT

public:
    /// Constructs a scale factor calibration state
    /// \param pDocument The shot group document to store the scale factor in
    /// \param pView The target view (provides access to the target scene for visual feedback)
    ScaleFactorState(ShotGroupDocument* pDocument, TargetView* pView);
    ~ScaleFactorState();

    /// Resets the calibration state and prepares for user interaction
    void onEnter() override;
    
    /// Hides the scale line visual feedback when leaving this state
    void onExit() override;
    
    /// Handles two-click calibration sequence
    ///
    /// First click sets the start point of the reference line. Second click
    /// completes the calibration by calculating the scale factor based on the
    /// pixel distance between the two points and the user-entered real-world
    /// distance. The scale factor is stored in the document and the state
    /// signals completion.
    /// \param scenePos The mouse click position in scene coordinates
    void handleMouseClick(const QPointF& scenePos) override;
    
    /// Updates the visual scale line preview during mouse movement
    ///
    /// When the first point has been set, this updates the end point of the
    /// scale line to follow the mouse cursor, providing real-time visual
    /// feedback of the calibration distance.
    /// \param scenePos The current mouse position in scene coordinates
    void handleMouseMove(const QPointF& scenePos) override;
    
    /// Returns true if the document has a valid scale factor set
    bool isComplete() const override;
    
    /// Returns a crosshair cursor for precise point selection
    QCursor cursor() const override;
    
    /// Populates the toolbar with distance input controls
    ///
    /// Adds a label, a double spinbox for entering the reference distance
    /// (in inches), and a unit label. The spinbox accepts values from 0.1
    /// to 100.0 inches with 2 decimal precision.
    /// \param toolbar The toolbar to populate with state-specific controls
    void populateToolbar(QToolBar* pToolbar) override;
    
    /// Returns the display name for this state: "Scale Factor"
    QString stateName() const override;

    /// Updates the document and resets internal state.
    void setDocument(ShotGroupDocument* pDocument) override;

private:
    void calculateScaleFactor();
    void reset();

private:
    TargetView* m_pView;        ///< The target view widget (provides access to the target scene)
    
    QPointF m_firstPoint;      ///< The first clicked point (start of reference line)
    bool m_hasFirstPoint = false;  ///< Whether the first point has been set
    
    QDoubleSpinBox* m_pDistanceSpin = nullptr;  ///< Spinbox for entering reference distance in inches
};
