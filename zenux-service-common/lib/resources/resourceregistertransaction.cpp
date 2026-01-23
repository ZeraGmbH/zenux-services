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
    m_msgNrListRegister.append(msgnr);
    m_rmConnection->SendCommand(cmd, registerParameter, msgnr);
}

void ResourceRegisterTransaction::resourceManagerAck(quint32 msgnr)
{
    if (m_msgNrListRegister.removeOne(msgnr) && m_msgNrListRegister.isEmpty())
        emit registerRdy();
}
