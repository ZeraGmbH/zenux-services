#include "statusinterface.h"
#include "protonetcommand.h"
#include "permissionfunctions.h"
#include <scpi.h>

enum StatusCommands
{
    cmdDevice,
    cmdAdjustment,
    cmdAuthorization
};

cStatusInterface::cStatusInterface(cSCPI *scpiInterface, AdjustmentStatusInterface *adjustmentStatusInterface) :
    ScpiConnection(scpiInterface),
    m_adjustmentStatusInterface(adjustmentStatusInterface)
{
}

void cStatusInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    cSCPIDelegate* delegate;
    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"DEVICE",SCPI::isQuery, m_pSCPIInterface, cmdDevice);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &cStatusInterface::executeCommand);
    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"ADJUSTMENT", SCPI::isQuery, m_pSCPIInterface, cmdAdjustment);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &cStatusInterface::executeCommand);
    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"AUTHORIZATION", SCPI::isQuery, m_pSCPIInterface, cmdAuthorization);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &cStatusInterface::executeCommand);
}

void cStatusInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery()) {
        switch (cmdCode)
        {
        case cmdDevice:
            protoCmd->m_sOutput = QString("%1").arg(getDeviceStatus());
            break;
        case cmdAdjustment:
            protoCmd->m_sOutput = QString("%1").arg(m_adjustmentStatusInterface->getAdjustmentStatus());
            break;
        case cmdAuthorization:
            protoCmd->m_sOutput = QString("%1").arg(getAuthorizationStatus());
            break;
        }
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

quint8 cStatusInterface::getDeviceStatus()
{
    bool enable;
    if (PermissionFunctions::checkControllerPin(enable)) // no problem reading from atmel
        return 1; // means device available
    else
        return 0;
}

quint8 cStatusInterface::getAuthorizationStatus()
{
    quint8 ret = 0;
    bool enable;
    if (PermissionFunctions::checkControllerPin(enable) && enable)
        ret = 1;
    return ret;
}
