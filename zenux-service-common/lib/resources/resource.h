#ifndef RESOURCE_H
#define RESOURCE_H

#include "rmconnection.h"
#include "scpiconnection.h"
#include <QList>

// pure virtual base class for resources to register themselves anywhere
class cResource: public ScpiConnection
{
    Q_OBJECT
public:
    cResource(cSCPI *scpiInterface);
    virtual ~cResource();
    virtual void registerResource(RMConnection *rmConnection, quint16 port) = 0;
    virtual void unregisterResource(RMConnection *rmConnection) = 0;
signals:
    void registerRdy(); // we emit signal when all register or unregister action is done
protected:
    QList<quint32> msgNrList;
    void register1Resource(RMConnection *rmConnection, quint32 msgnr, QString registerParameter);
    void unregister1Resource(RMConnection *rmConnection, quint32 msgnr, QString unregisterParameter);
protected slots:
    void resourceManagerAck(quint32 msgnr);
};

#endif // RESOURCE_H
