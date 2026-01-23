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
    cResource(std::shared_ptr<cSCPI> scpiInterface);
    virtual ~cResource() = default;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) = 0;
signals:
    void registerRdy();
    void unregisterRdy();
public slots:
    void resourceManagerAck(quint32 msgnr);
protected:
    void register1Resource(RMConnection *rmConnection, quint32 msgnr, QString registerParameter);
    void unregister1Resource(RMConnection *rmConnection, quint32 msgnr, QString unregisterParameter);
private:
    QList<quint32> m_msgNrListRegister;
    QList<quint32> m_msgNrListUnregister;
};

#endif // RESOURCE_H
