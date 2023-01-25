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
    addDelegate(new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"DEVICE",SCPI::isQuery, m_pSCPIInterface, cmdDevice));
    addDelegate(new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"ADJUSTMENT", SCPI::isQuery, m_pSCPIInterface, cmdAdjustment));
    addDelegate(new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"AUTHORIZATION", SCPI::isQuery, m_pSCPIInterface, cmdAuthorization));
}

void cStatusInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery()) {
        switch (cmdCode)
        {
        case cmdDevice:
            protoCmd->m_sOutput = getDeviceStatus();
            break;
        case cmdAdjustment:
            protoCmd->m_sOutput = QString("%1").arg(m_adjustmentStatusInterface->getAdjustmentStatus());
            break;
        case cmdAuthorization:
            emit strNotifier(&m_notifierAutorization);
            protoCmd->m_sOutput = getAuthorizationStatus();
            break;
        }
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString cStatusInterface::getDeviceStatus()
{
    bool enable;
    if (PermissionFunctions::checkControllerPin(enable)) // no problem reading from atmel
        return "1"; // means device available
    else
        return "0";
}

QString cStatusInterface::getAuthorizationStatus()
{
    QString status = "0";
    bool enable;
    if (PermissionFunctions::checkControllerPin(enable) && enable)
        status = "1";
    m_notifierAutorization = status;
    return status;
}
