#include <QX/Services.h>
#include "QServiceLocator.h"

#include <stdexcept>

namespace
{
    QServiceLocator* g_pServiceLocator = nullptr;

    QServiceLocator& RequireLocator()
    {
        if (!g_pServiceLocator)
        {
            throw std::logic_error("Service locator has not been initialized");
        }
        return *g_pServiceLocator;
    }
}

namespace qx
{
    void InitServiceLocator()
    {
        if (g_pServiceLocator)
        {
            return;
        }

        g_pServiceLocator = new QServiceLocator();
    }

    void ShutdownServiceLocator()
    {
        delete g_pServiceLocator;
        g_pServiceLocator = nullptr;
    }

    namespace detail
    {
        void RegisterService(const QMetaObject* pMetaObject, QService* pService)
        {
            RequireLocator().registerService(pMetaObject, pService);
        }

        QService& GetService(const QMetaObject* pMetaObject)
        {
            return RequireLocator().getService(pMetaObject);
        }
    }
}
