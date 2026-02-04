#pragma once

#include <QUndoCommand>
#include <Core/ShotImpact.h>

class ShotGroupDocument;
class TargetScene;

/// @brief Undo command for removing an impact from the document and scene
/// 
/// This command implements the undo/redo functionality for removing a shot impact.
/// It maintains synchronization between the document model (ShotGroupDocument) and
/// the visual representation (TargetScene). When executed, it removes the impact
/// from both the document's data model and the scene's visual glyphs. The command
/// stores the impact data and diameter to allow proper restoration during undo.
class RemoveImpactCommand : public QUndoCommand
{
public:
    /// @brief Constructs a command to remove an impact
    /// 
    /// The command text is automatically set to "Remove Impact {id}" where {id}
    /// is the impact's unique identifier.
    /// 
    /// @param document The document containing the impact data model. Must not be null.
    /// @param scene The scene containing the visual impact glyphs. Must not be null.
    /// @param impact The impact to remove. A copy is stored for undo operations.
    /// @param diameterPixels The visual diameter of the impact glyph in pixels,
    ///                       required to restore the glyph during undo.
    /// @param parent Optional parent command for command grouping.
    RemoveImpactCommand(ShotGroupDocument* pDocument, TargetScene* pScene,
                        const ShotImpact& impact, double diameterPixels,
                        QUndoCommand* pParent = nullptr);
    
    ~RemoveImpactCommand();

    /// @brief Restores the removed impact to both document and scene
    /// 
    /// Adds the impact back to the document model and recreates its visual
    /// glyph in the scene using the stored position and diameter.
    void undo() override;
    
    /// @brief Removes the impact from both document and scene
    /// 
    /// Searches the document for an impact matching the stored ID and removes it
    /// by index. Also removes the corresponding visual glyph from the scene.
    /// This operation is idempotent - if the impact is already removed, no
    /// error occurs.
    void redo() override;

private:
    ShotGroupDocument* m_pDocument;  ///< The document model containing impact data
    TargetScene* m_pScene;            ///< The scene containing visual impact glyphs
    ShotImpact m_impact;             ///< Copy of the impact being removed (for undo)
    double m_diameterPixels;         ///< Visual diameter in pixels (needed to restore glyph)
};
