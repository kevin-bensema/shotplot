#pragma once

#include <QUndoCommand>
#include <Core/ShotImpact.h>

class ShotGroupDocument;

/// @brief Undo command for adding a shot impact to the document.
///
/// This command implements the undo/redo functionality for adding a new impact
/// to a shot group. When executed (redo), it adds the impact to the document
/// model. When undone, it removes the impact by matching the impact ID.
/// The scene is automatically synchronized via the impactsChanged signal.
class AddImpactCommand : public QUndoCommand
{
public:
    /// @brief Constructs an AddImpactCommand.
    ///
    /// Creates a new undo command for adding an impact. The command text is
    /// automatically set to "Add Impact <id>" for display in the undo stack.
    ///
    /// \param document The document model to which the impact will be added.
    /// \param impact The impact data to add (must have a valid ID).
    /// \param parent Optional parent command for command grouping.
    AddImpactCommand(ShotGroupDocument* pDocument,
                     const ShotImpact &impact,
                     QUndoCommand* pParent = nullptr);
    ~AddImpactCommand();

    /// @brief Removes the impact from the document.
    ///
    /// Searches for the impact by ID in the document's impact list and removes it.
    void undo() override;

    /// @brief Adds the impact to the document.
    void redo() override;

private:
    ShotGroupDocument* m_pDocument;  ///< The document model managing shot data
    ShotImpact m_impact;             ///< The impact data to add/remove
};
