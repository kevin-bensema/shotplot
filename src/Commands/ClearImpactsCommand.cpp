#include "ClearImpactsCommand.h"
#include <Core/ShotGroupDocument.h>

ClearImpactsCommand::ClearImpactsCommand(ShotGroupDocument* pDocument,
                                         QUndoCommand* pParent)
    : QUndoCommand(pParent)
    , m_pDocument(pDocument)
{
    setText(QObject::tr("Clear All Impacts"));
    m_savedImpacts = m_pDocument->impacts();
}

ClearImpactsCommand::~ClearImpactsCommand() = default;

void ClearImpactsCommand::undo()
{
    m_pDocument->replaceImpacts(m_savedImpacts);
}

void ClearImpactsCommand::redo()
{
    m_pDocument->clearImpacts();
}
