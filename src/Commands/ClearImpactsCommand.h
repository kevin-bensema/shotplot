#pragma once

#include <QUndoCommand>
#include <QList>
#include <Core/ShotImpact.h>

class ShotGroupDocument;

/// @brief Undo command for clearing all impacts from a shot group document.
///
/// This command provides undo/redo functionality for clearing all shot impacts
/// from the document model. On construction, it saves the current state of all
/// impacts so they can be restored during undo. The scene is automatically
/// synchronized via the impactsChanged signal.
class ClearImpactsCommand : public QUndoCommand
{
public:
    /// Constructs a command to clear all impacts from the document.
    ///
    /// The constructor saves the current impacts so they can be restored on undo.
    ///
    /// \param document The document containing the impacts to clear.
    /// \param parent Optional parent command for command grouping.
    ClearImpactsCommand(ShotGroupDocument* pDocument,
                        QUndoCommand* pParent = nullptr);

    ~ClearImpactsCommand();

    /// Restores all previously saved impacts to the document.
    void undo() override;

    /// Clears all impacts from the document.
    void redo() override;

private:
    ShotGroupDocument* m_pDocument;   ///< The document containing impacts to clear.
    QList<ShotImpact> m_savedImpacts; ///< Snapshot of impacts saved for undo restoration.
};
