#pragma once

#include <QX/Export.h>
#include <QX/QService.h>

#include <type_traits>

class QMetaObject;

namespace qx
{
    /// Initializes the global service locator. Idempotent; if the service
    /// locator has already been created, this function does nothing.
    QX_EXPORT void InitServiceLocator();

    /// Destroys the global service locator and all owned services.
    /// After calling this, InitServiceLocator() may be called again to
    /// create a fresh service locator.
    QX_EXPORT void ShutdownServiceLocator();

    /// \cond INTERNAL
    namespace detail
    {
        QX_EXPORT void RegisterService(const QMetaObject* pMetaObject, QService* pService);
        QX_EXPORT QService& GetService(const QMetaObject* pMetaObject);
    }
    /// \endcond

    /// Registers a service instance with the global service locator.
    /// The service locator takes ownership of the service.
    ///
    /// \tparam T A type derived from QService.
    /// \param pService Pointer to the service instance to register.
    /// \throws std::logic_error if a service of type T is already registered.
    template<typename T>
    void RegisterService(T* pService)
    {
        static_assert(std::is_base_of_v<QService, T>, "T must inherit from QService");
        detail::RegisterService(&T::staticMetaObject, pService);
    }

    /// Returns a reference to the registered service of type T.
    ///
    /// \tparam T A type derived from QService.
    /// \return Reference to the service instance.
    /// \throws std::logic_error if no service of type T is registered.
    template<typename T>
    T& GetService()
    {
        static_assert(std::is_base_of_v<QService, T>, "T must inherit from QService");
        return static_cast<T&>(detail::GetService(&T::staticMetaObject));
    }
}
