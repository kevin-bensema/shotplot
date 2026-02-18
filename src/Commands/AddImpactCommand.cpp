#include "AddImpactCommand.h"
#include <Core/ShotGroupDocument.h>

AddImpactCommand::AddImpactCommand(ShotGroupDocument* pDocument,
                                   const ShotImpact &impact,
                                   QUndoCommand* pParent)
    : QUndoCommand(pParent)
    , m_pDocument(pDocument)
    , m_impact(impact)
{
    setText(QObject::tr("Add Impact %1").arg(impact.id));
}

AddImpactCommand::~AddImpactCommand() = default;

void AddImpactCommand::undo()
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

void AddImpactCommand::redo()
{
    m_pDocument->addImpact(m_impact);
}
