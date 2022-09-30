#include "hkeychannel.h"
#include <scpiconnection.h>
#include <protonetcommand.h>
#include <scpicommand.h>

cHKeyChannel::cHKeyChannel(cSCPI *scpiinterface, QString description, quint8 nr, HKeySystem::cChannelSettings *cSettings) :
    cSCPIConnection(scpiinterface),
    m_sDescription(description)
{
    m_sName = QString("hk%1").arg(nr);
    m_sAlias = cSettings->m_sAlias;
    m_bAvail = cSettings->avail;
}

void cHKeyChannel::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;
    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, HKEYChannel::cmdAlias);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, HKEYChannel::cmdStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}


void cHKeyChannel::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case HKEYChannel::cmdAlias:
        protoCmd->m_sOutput = m_ReadAlias(protoCmd->m_sInput);
        break;
    case HKEYChannel::cmdStatus:
        protoCmd->m_sOutput = m_ReadChannelStatus(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString &cHKeyChannel::getName()
{
    return m_sName;
}


QString &cHKeyChannel::getAlias()
{
    return m_sAlias;
}


QString &cHKeyChannel::getDescription()
{
    return m_sDescription;
}


bool cHKeyChannel::isAvail()
{
    return m_bAvail;
}


QString cHKeyChannel::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cHKeyChannel::m_ReadChannelStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        quint32 r;
        r = ((m_bAvail) ? 0 : 1 << 31);
            return QString("%1").arg(r);
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

