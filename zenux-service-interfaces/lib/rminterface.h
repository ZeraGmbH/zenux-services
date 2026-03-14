#ifndef RMINTERFACE_H
#define RMINTERFACE_H

#include "service-interfaces_export.h"
#include "abstractserverInterface.h"
#include <proxyclient.h>
#include <memory>

namespace Zera {

class cRMInterfacePrivate;

class SERVICE_INTERFACES_EXPORT cRMInterface: public AbstractServerInterface
{
    Q_OBJECT
public:
    cRMInterface();
    virtual ~cRMInterface();
    void setClientSmart(Zera::ProxyClientPtr client) override;
    quint32 scpiCommand(const QString &scpi) override;

    quint32 rmIdent(const QString &name);
    quint32 addResource(const QString &type, const QString &name, int n, const QString &description, quint16 port);
    quint32 removeResource(const QString &type, const QString &name);
    quint32 getResourceTypes();
    quint32 getResources(const QString &type);
    quint32 getResourceInfo(const QString &type, const QString &name);
    quint32 setResource(const QString &type, const QString &name, int n);
    quint32 freeResource(const QString &type, const QString &name);

private:
    Q_DECLARE_PRIVATE(cRMInterface)
    cRMInterfacePrivate *d_ptr;
};

typedef std::shared_ptr<cRMInterface> RMInterfacePtr;

}

#endif // RMINTERFACE_H
