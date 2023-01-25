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
    cSCPIDelegate* delegate;
    delegate = new cSCPIDelegate(QString("%1SERVER").arg(leadingNodes), "REGISTER", SCPI::isCmdwP, m_pSCPIInterface, cmdRegister);
    m_DelegateList.append(delegate);
    //connect(delegate, &cSCPIDelegate::execute, this, &AsynchNotificationsInterface::executeCommand);
    delegate = new cSCPIDelegate(QString("%1SERVER").arg(leadingNodes), "UNREGISTER", SCPI::isCmdwP, m_pSCPIInterface, cmdUnregister);
    m_DelegateList.append(delegate);
    //connect(delegate, &cSCPIDelegate::execute, this, &AsynchNotificationsInterface::executeCommand);
}

void AsynchNotificationsInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{

}
