#include "resource.h"
#include "rmconnection.h"
#include <QString>

cResource::cResource(std::shared_ptr<cSCPI> scpiInterface) :
    ScpiConnection(scpiInterface)
{
}

void cResource::register1Resource(RMConnection *rmConnection, quint32 msgnr, QString registerParameter)
{
    QString cmd = QString("RESOURCE:ADD");
    m_msgNrListRegister.append(msgnr);
    rmConnection->SendCommand(cmd, registerParameter, msgnr);
}

void cResource::unregister1Resource(RMConnection *rmConnection, quint32 msgnr, QString unregisterParameter)
{
    QString cmd = QString("RESOURCE:REMOVE");
    m_msgNrListUnregister.append(msgnr);
    rmConnection->SendCommand(cmd, unregisterParameter, msgnr);
}

void cResource::resourceManagerAck(quint32 msgnr)
{
    if(m_msgNrListRegister.removeOne(msgnr) && m_msgNrListRegister.isEmpty())
        emit registerRdy();
    if(m_msgNrListUnregister.removeOne(msgnr) && m_msgNrListUnregister.isEmpty())
        emit unregisterRdy();
}
