#include "fpzinchannel.h"
#include "scpiconnection.h"
#include "protonetcommand.h"
#include "settings/frqinputsettings.h"
#include <scpi.h>

cFPZInChannel::cFPZInChannel(cSCPI *scpiInterface, QString description, quint8 nr, FRQInputSystem::cChannelSettings *cSettings) :
    ScpiConnection(scpiInterface),
    m_sDescription(description)
{
    m_sName = QString("fi%1").arg(nr);
    m_sAlias = cSettings->m_sAlias;
    m_bAvail = cSettings->avail;
}

void cFPZInChannel::initSCPIConnection(QString leadingNodes)
{
    if (leadingNodes != "")
        leadingNodes += ":";
    cSCPIDelegate* delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, FPZINChannel::cmdAlias);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &cFPZInChannel::executeCommand);
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, FPZINChannel::cmdStatus);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &cFPZInChannel::executeCommand);
}

void cFPZInChannel::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
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

QString &cFPZInChannel::getName()
{
    return m_sName;
}

QString &cFPZInChannel::getAlias()
{
    return m_sAlias;
}

QString &cFPZInChannel::getDescription()
{
    return m_sDescription;
}

bool cFPZInChannel::isAvail()
{
    return m_bAvail;
}

QString cFPZInChannel::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString cFPZInChannel::m_ReadChannelStatus(QString &sInput)
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
