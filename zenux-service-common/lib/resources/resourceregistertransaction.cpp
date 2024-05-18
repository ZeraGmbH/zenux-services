#include "resourceregistertransaction.h"
#include "notzeronumgen.h"

ResourceRegisterTransaction::ResourceRegisterTransaction(RMConnection *rmConnection) :
    m_rmConnection(rmConnection)
{
    connect(m_rmConnection, &RMConnection::rmAck,
            this, &ResourceRegisterTransaction::resourceManagerAck);
}

void ResourceRegisterTransaction::register1Resource(QString registerParameter)
{
    QString cmd = QString("RESOURCE:ADD");
    quint32 msgnr = NotZeroNumGen::getMsgNr();
    m_msgNrList.append(msgnr);
    m_rmConnection->SendCommand(cmd, registerParameter, msgnr);
}

void ResourceRegisterTransaction::resourceManagerAck(quint32 msgnr)
{
    if (m_msgNrList.removeOne(msgnr) && m_msgNrList.isEmpty())
        emit registerDone();
}
