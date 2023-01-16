#include "fpzinchannelinterface.h"
#include "scpiconnection.h"
#include "protonetcommand.h"
#include "settings/frqinputsettings.h"
#include <scpi.h>

FpzInChannelInterface::FpzInChannelInterface(cSCPI *scpiInterface, QString description, quint8 nr, FRQInputSystem::cChannelSettings *cSettings) :
    ScpiConnection(scpiInterface),
    m_sDescription(description)
{
    m_sName = QString("fi%1").arg(nr);
    m_sAlias = cSettings->m_sAlias;
    m_bAvail = cSettings->avail;
}

void FpzInChannelInterface::initSCPIConnection(QString leadingNodes)
{
    if (leadingNodes != "")
        leadingNodes += ":";
    cSCPIDelegate* delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, FPZINChannel::cmdAlias);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &FpzInChannelInterface::executeCommand);
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, FPZINChannel::cmdStatus);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &FpzInChannelInterface::executeCommand);
}

void FpzInChannelInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case FPZINChannel::cmdAlias:
        protoCmd->m_sOutput = m_ReadAlias(protoCmd->m_sInput);
        break;
    case FPZINChannel::cmdStatus:
        protoCmd->m_sOutput = m_ReadChannelStatus(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString &FpzInChannelInterface::getName()
{
    return m_sName;
}

QString &FpzInChannelInterface::getAlias()
{
    return m_sAlias;
}

QString &FpzInChannelInterface::getDescription()
{
    return m_sDescription;
}

bool FpzInChannelInterface::isAvail()
{
    return m_bAvail;
}

QString FpzInChannelInterface::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FpzInChannelInterface::m_ReadChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint32 r;
        r = ((m_bAvail) ? 0 : 1 << 31);
            return QString("%1").arg(r);
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}
