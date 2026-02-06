#include "QServiceLocator.h"

#include <stdexcept>

QServiceLocator::QServiceLocator(QObject* pParent)
    : QObject(pParent)
{
}

void QServiceLocator::registerService(const QMetaObject* pMetaObject, QService* pService)
{
    if (m_services.contains(pMetaObject))
    {
        throw std::logic_error(
            std::string("Service already registered: ") + pMetaObject->className());
    }

    pService->setParent(this);
    m_services.insert(pMetaObject, pService);
}

QService& QServiceLocator::getService(const QMetaObject* pMetaObject) const
{
    auto it = m_services.find(pMetaObject);
    if (it == m_services.end())
    {
        throw std::logic_error(
            std::string("Service not registered: ") + pMetaObject->className());
    }

    return *it.value();
}
