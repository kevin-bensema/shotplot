#include "AddImpactCommand.h"
#include "Core/ShotGroupDocument.h"
#include "Graphics/TargetScene.h"

AddImpactCommand::AddImpactCommand(ShotGroupDocument *document, TargetScene *scene,
                                   const ShotImpact &impact, double diameterPixels,
                                   QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_scene(scene)
    , m_impact(impact)
    , m_diameterPixels(diameterPixels)
{
    setText(QObject::tr("Add Impact %1").arg(impact.id));
}

AddImpactCommand::~AddImpactCommand() = default;

void AddImpactCommand::undo()
{
    // Find and remove the impact
    const auto &impacts = m_document->impacts();
    for (int i = 0; i < impacts.size(); ++i)
    {
        if (impacts[i].id == m_impact.id)
        {
            m_document->removeImpact(i);
            break;
        }
    }
    
    m_scene->removeImpactGlyph(m_impact.id);
}

void AddImpactCommand::redo()
{
    m_document->addImpact(m_impact);
    m_scene->addImpactGlyph(m_impact.id, m_impact.position(), m_diameterPixels);
}
