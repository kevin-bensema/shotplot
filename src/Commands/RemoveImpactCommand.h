#pragma once

#include <QUndoCommand>
#include <Core/ShotImpact.h>

class ShotGroupDocument;

/// @brief Undo command for removing an impact from the document
///
/// This command implements the undo/redo functionality for removing a shot impact.
/// When executed, it removes the impact from the document model. When undone, it
/// restores the impact. The scene is automatically synchronized via the
/// impactsChanged signal.
class RemoveImpactCommand : public QUndoCommand
{
public:
    /// @brief Constructs a command to remove an impact
    ///
    /// The command text is automatically set to "Remove Impact {id}" where {id}
    /// is the impact's unique identifier.
    ///
    /// @param document The document containing the impact data model. Must not be null.
    /// @param impact The impact to remove. A copy is stored for undo operations.
    /// @param parent Optional parent command for command grouping.
    RemoveImpactCommand(ShotGroupDocument* pDocument,
                        const ShotImpact& impact,
                        QUndoCommand* pParent = nullptr);

    ~RemoveImpactCommand();

    /// @brief Restores the removed impact to the document.
    void undo() override;

    /// @brief Removes the impact from the document.
    ///
    /// Searches the document for an impact matching the stored ID and removes it.
    void redo() override;

private:
    ShotGroupDocument* m_pDocument;  ///< The document model containing impact data
    ShotImpact m_impact;             ///< Copy of the impact being removed (for undo)
};
