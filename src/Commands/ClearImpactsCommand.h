#pragma once

#include <QUndoCommand>
#include <QList>
#include <Core/ShotImpact.h>

class ShotGroupDocument;
class TargetScene;

/// @brief Undo command for clearing all impacts from a shot group document.
///
/// This command provides undo/redo functionality for clearing all shot impacts
/// from both the document model and the visual scene. On construction, it saves
/// the current state of all impacts so they can be restored during undo.
///
/// The command coordinates changes between ShotGroupDocument (data model) and
/// TargetScene (visual representation) to ensure both remain synchronized.
class ClearImpactsCommand : public QUndoCommand
{
public:
    /// Constructs a command to clear all impacts from the document and scene.
    ///
    /// The constructor saves the current impacts and calculates the impact
    /// diameter in pixels for proper restoration during undo. The diameter
    /// calculation uses the document's bullet diameter and pixels-per-inch
    /// scale factor if available, otherwise defaults to 20.0 pixels.
    ///
    /// \param document The document containing the impacts to clear.
    /// \param scene The visual scene displaying impact glyphs.
    /// \param parent Optional parent command for command grouping.
    ClearImpactsCommand(ShotGroupDocument* pDocument, TargetScene* pScene,
                        QUndoCommand* pParent = nullptr);
    
    ~ClearImpactsCommand();

    /// Restores all previously saved impacts to the document and scene.
    ///
    /// Re-adds all impacts that were saved during construction, restoring
    /// both the document data model and the visual glyphs in the scene.
    void undo() override;
    
    /// Clears all impacts from the document and scene.
    ///
    /// Removes all impact glyphs from the visual scene and clears all impacts
    /// from the document data model.
    void redo() override;

private:
    ShotGroupDocument* m_pDocument;  ///< The document containing impacts to clear.
    TargetScene* m_pScene;           ///< The visual scene displaying impact glyphs.
    QList<ShotImpact> m_savedImpacts; ///< Snapshot of impacts saved for undo restoration.
    double m_diameterPixels;  ///< Impact diameter in pixels, calculated from bullet diameter and scale.
};
