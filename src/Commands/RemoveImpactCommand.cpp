#include "RemoveImpactCommand.h"
#include "Core/ShotGroupDocument.h"
#include "Graphics/TargetScene.h"

RemoveImpactCommand::RemoveImpactCommand(ShotGroupDocument *document, TargetScene *scene,
                                         const ShotImpact &impact, double diameterPixels,
                                         QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_document(document)
    , m_scene(scene)
    , m_impact(impact)
    , m_diameterPixels(diameterPixels)
{
    setText(QObject::tr("Remove Impact %1").arg(impact.id));
}

RemoveImpactCommand::~RemoveImpactCommand() = default;

void RemoveImpactCommand::undo()
{
    m_document->addImpact(m_impact);
    m_scene->addImpactGlyph(m_impact.id, m_impact.position(), m_diameterPixels);
}

void RemoveImpactCommand::redo()
{
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
