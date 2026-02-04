#include "ClearImpactsCommand.h"
#include <Core/ShotGroupDocument.h>
#include <Graphics/TargetScene.h>

namespace {
    constexpr double kDefaultDiameterPixels = 20.0;
}

ClearImpactsCommand::ClearImpactsCommand(ShotGroupDocument* pDocument, TargetScene* pScene,
                                         QUndoCommand* pParent)
    : QUndoCommand(pParent)
    , m_pDocument(pDocument)
    , m_pScene(pScene)
    , m_diameterPixels(kDefaultDiameterPixels)
{
    setText(QObject::tr("Clear All Impacts"));
    
    // Save current impacts for undo
    m_savedImpacts = m_pDocument->impacts();
    
    // Calculate diameter for restoration
    if (m_pDocument->hasScaleFactorSet() && m_pDocument->bulletDiameter() > 0)
    {
        m_diameterPixels = m_pDocument->bulletDiameter() * m_pDocument->pixelsPerInch();
    }
}

ClearImpactsCommand::~ClearImpactsCommand() = default;

void ClearImpactsCommand::undo()
{
    // Restore all saved impacts
    m_pDocument->replaceImpacts(m_savedImpacts);
    
    for (const auto &impact : m_savedImpacts)
    {
        m_pScene->addImpactGlyph(impact.id, impact.position(), m_diameterPixels);
    }
}

void ClearImpactsCommand::redo()
{
    m_pScene->clearImpactGlyphs();
    m_pDocument->clearImpacts();
}
