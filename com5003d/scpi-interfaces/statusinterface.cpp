#include "adjustment.h"
#include "statusinterface.h"
#include <protonetcommand.h>
#include "micro-controller-io/atmel.h"

extern cATMEL* pAtmel;


cStatusInterface::cStatusInterface(cCOM5003dServer* server) :
    cSCPIConnection(server->getSCPIInterface()),
    m_pMyServer(server)
{
}


void cStatusInterface::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"DEVICE",SCPI::isQuery,m_pSCPIInterface, StatusSystem::cmdDevice);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"ADJUSTMENT", SCPI::isQuery, m_pSCPIInterface, StatusSystem::cmdAdjustment);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"AUTHORIZATION", SCPI::isQuery, m_pSCPIInterface, StatusSystem::cmdAuthorization);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}


void cStatusInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isQuery())
    {
        switch (cmdCode)
        {
        case StatusSystem::cmdDevice:
            protoCmd->m_sOutput = QString("%1").arg(getDeviceStatus());
            break; // StatusDevice
        case StatusSystem::cmdAdjustment:
            protoCmd->m_sOutput = QString("%1").arg(m_pMyServer->m_pAdjHandler->getAdjustmentStatus());
            break; // StatusAdjustment
        case StatusSystem::cmdAuthorization:
            protoCmd->m_sOutput = QString("%1").arg(getAuthorizationStatus());
            break; // StatusAuthorization
        }
    }
    else
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


quint8 cStatusInterface::getDeviceStatus()
{
    QString s;

    if (pAtmel->readDeviceName(s) == ZeraMcontrollerBase::cmddone) // no problem reading from atmel
        return 1; // means device available
    else
        return 0;
}


quint8 cStatusInterface::getAuthorizationStatus()
{
    quint8 ret;
    bool enable;

    ret  = 0;
    if (pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone)
    {
        if (enable)
            ret = 1;
    }

    return ret;
}
