#include "ClearImpactsCommand.h"
#include "Core/ShotGroupDocument.h"
#include "Graphics/TargetScene.h"

ClearImpactsCommand::ClearImpactsCommand(ShotGroupDocument *document, TargetScene *scene,
                                         QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_scene(scene)
{
    setText(QObject::tr("Clear All Impacts"));
    
    // Save current impacts for undo
    m_savedImpacts = m_document->impacts();
    
    // Calculate diameter for restoration
    if (m_document->hasScaleFactorSet() && m_document->bulletDiameter() > 0)
    {
        m_diameterPixels = m_document->bulletDiameter() * m_document->pixelsPerInch();
    }
}

ClearImpactsCommand::~ClearImpactsCommand() = default;

void ClearImpactsCommand::undo()
{
    // Restore all saved impacts
    m_document->replaceImpacts(m_savedImpacts);
    
    for (const auto &impact : m_savedImpacts)
    {
        m_scene->addImpactGlyph(impact.id, impact.position(), m_diameterPixels);
    }
}

void ClearImpactsCommand::redo()
{
    m_scene->clearImpactGlyphs();
    m_document->clearImpacts();
}
