#pragma once

#include <QX/QService.h>

#include <QHash>
#include <QObject>

class QMetaObject;

/// Internal service locator that owns and manages registered services.
///
/// This class is not part of the public QX API. Clients should use the
/// free functions in the qx namespace (Services.h) instead.
class QServiceLocator : public QObject
{
    Q_OBJECT

public:
    explicit QServiceLocator(QObject* pParent = nullptr);

    /// Registers a service keyed by its QMetaObject.
    /// Takes ownership of the service by reparenting it.
    ///
    /// \param pMetaObject The meta object identifying the service type.
    /// \param pService The service instance to register.
    /// \throws std::logic_error if a service for this type is already registered.
    void registerService(const QMetaObject* pMetaObject, QService* pService);

    /// Returns a reference to the service identified by the given QMetaObject.
    ///
    /// \param pMetaObject The meta object identifying the service type.
    /// \return Reference to the registered service.
    /// \throws std::logic_error if no service for this type is registered.
    QService& getService(const QMetaObject* pMetaObject) const;

private:
    QHash<const QMetaObject*, QService*> m_services;
};
