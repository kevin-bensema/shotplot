#include "RemoveImpactCommand.h"
#include "Core/ShotGroupDocument.h"
#include "Graphics/TargetScene.h"

RemoveImpactCommand::RemoveImpactCommand(ShotGroupDocument* pDocument, TargetScene* pScene,
                                         const ShotImpact &impact, double diameterPixels,
                                         QUndoCommand* pParent)
    : QUndoCommand(pParent)
    , m_pDocument(pDocument)
    , m_pScene(pScene)
    , m_impact(impact)
    , m_diameterPixels(diameterPixels)
{
    setText(QObject::tr("Remove Impact %1").arg(impact.id));
}

RemoveImpactCommand::~RemoveImpactCommand() = default;

void RemoveImpactCommand::undo()
{
    m_pDocument->addImpact(m_impact);
    m_pScene->addImpactGlyph(m_impact.id, m_impact.position(), m_diameterPixels);
}

void RemoveImpactCommand::redo()
{
    const auto& impacts = m_pDocument->impacts();
    for (int i = 0; i < impacts.size(); ++i)
    {
        if (impacts[i].id == m_impact.id)
        {
            m_pDocument->removeImpact(i);
            break;
        }
    }
    
    m_pScene->removeImpactGlyph(m_impact.id);
}
