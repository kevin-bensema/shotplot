#pragma once

#include <QUndoCommand>
#include "Core/ShotImpact.h"

class ShotGroupDocument;
class TargetScene;

/// @brief Undo command for adding a shot impact to the document and scene.
///
/// This command implements the undo/redo functionality for adding a new impact
/// to a shot group. When executed (redo), it adds the impact to both the
/// document model and the visual scene. When undone, it removes the impact
/// from both locations by matching the impact ID.
///
/// The command stores the impact data and its visual diameter (in pixels) to
/// ensure proper restoration of both the data model and visual representation.
class AddImpactCommand : public QUndoCommand
{
public:
    /// @brief Constructs an AddImpactCommand.
    ///
    /// Creates a new undo command for adding an impact. The command text is
    /// automatically set to "Add Impact <id>" for display in the undo stack.
    ///
    /// \param document The document model to which the impact will be added.
    /// \param scene The visual scene where the impact glyph will be displayed.
    /// \param impact The impact data to add (must have a valid ID).
    /// \param diameterPixels The visual diameter of the impact glyph in pixels.
    /// \param parent Optional parent command for command grouping.
    AddImpactCommand(ShotGroupDocument* pDocument, TargetScene* pScene,
                     const ShotImpact &impact, double diameterPixels,
                     QUndoCommand* pParent = nullptr);
    ~AddImpactCommand();

    /// @brief Removes the impact from both the document and scene.
    ///
    /// Searches for the impact by ID in the document's impact list and removes
    /// it, then removes the corresponding glyph from the scene.
    void undo() override;

    /// @brief Adds the impact to both the document and scene.
    ///
    /// Adds the impact to the document model and creates a visual glyph in
    /// the scene at the impact's position with the specified diameter.
    void redo() override;

private:
    ShotGroupDocument* m_pDocument;  ///< The document model managing shot data
    TargetScene* m_pScene;           ///< The visual scene displaying the target
    ShotImpact m_impact;             ///< The impact data to add/remove
    double m_diameterPixels;        ///< Visual diameter of the impact glyph in pixels
};
