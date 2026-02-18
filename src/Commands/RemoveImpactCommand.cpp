#include "RemoveImpactCommand.h"
#include <Core/ShotGroupDocument.h>

RemoveImpactCommand::RemoveImpactCommand(ShotGroupDocument* pDocument,
                                         const ShotImpact& impact,
                                         QUndoCommand* pParent)
    : QUndoCommand(pParent)
    , m_pDocument(pDocument)
    , m_impact(impact)
{
    setText(QObject::tr("Remove Impact %1").arg(impact.id));
}

RemoveImpactCommand::~RemoveImpactCommand() = default;

void RemoveImpactCommand::undo()
{
    m_pDocument->addImpact(m_impact);
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
}
