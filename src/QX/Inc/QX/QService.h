#pragma once

#include <QObject>
#include <QX/Export.h>

/// Abstract base class for services managed by the QX service locator.
///
/// All services registered with the service locator must inherit from
/// QService. The service locator takes ownership of registered services.
class QX_EXPORT QService : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    virtual ~QService() = default;
};
