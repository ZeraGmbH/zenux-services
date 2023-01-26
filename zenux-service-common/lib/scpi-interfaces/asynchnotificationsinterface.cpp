#include "asynchnotificationsinterface.h"

enum commands
{
    cmdRegister,
    cmdUnregister
};

AsynchNotificationsInterface::AsynchNotificationsInterface(cSCPI *scpiInterface) :
    ScpiConnection(scpiInterface)
{

}

void AsynchNotificationsInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SERVER").arg(leadingNodes), "REGISTER", SCPI::isCmdwP, m_pSCPIInterface, cmdRegister);
    addDelegate(QString("%1SERVER").arg(leadingNodes), "UNREGISTER", SCPI::isCmdwP, m_pSCPIInterface, cmdUnregister);
}

void AsynchNotificationsInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{

}
