#include <QString>

#include "resource.h"
#include "rmconnection.h"


cResource::cResource(cSCPI *scpiInterface) :
    cSCPIConnection(scpiInterface)
{
}


cResource::~cResource()
{
}


void cResource::register1Resource(cRMConnection *rmConnection, quint32 msgnr, QString registerParameter)
{
    QString cmd = QString("RESOURCE:ADD");
    msgNrList.append(msgnr);
    rmConnection->SendCommand(cmd, registerParameter, msgnr);
}


void cResource::unregister1Resource(cRMConnection *rmConnection, quint32 msgnr, QString unregisterParameter)
{
    QString cmd = QString("RESOURCE:REMOVE");
    msgNrList.append(msgnr);
    rmConnection->SendCommand(cmd, unregisterParameter, msgnr);
}


void cResource::resourceManagerAck(quint32 msgnr)
{
    if (msgNrList.contains(msgnr))
    {
        msgNrList.removeOne(msgnr);
        if (msgNrList.isEmpty())
            emit registerRdy();
    }
}

