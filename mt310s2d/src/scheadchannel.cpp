#include <scpiconnection.h>
#include "scheadchannel.h"
#include <protonetcommand.h>
#include "settings/scheadsettings.h"
#include <scpi.h>
#include <scpicommand.h>


cSCHeadChannel::cSCHeadChannel(cSCPI *scpiinterface, QString description, quint8 nr, SCHeadSystem::cChannelSettings *cSettings) :
    cSCPIConnection(scpiinterface),
    m_sDescription(description)
{
    m_sName = QString("sh%1").arg(nr);
    m_sAlias = cSettings->m_sAlias;
    m_bAvail = cSettings->avail;
}


void cSCHeadChannel::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, SCHEADChannel::cmdAlias);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, SCHEADChannel::cmdStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}


void cSCHeadChannel::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SCHEADChannel::cmdAlias:
        protoCmd->m_sOutput = m_ReadAlias(protoCmd->m_sInput);
        break;
    case SCHEADChannel::cmdStatus:
        protoCmd->m_sOutput = m_ReadChannelStatus(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString &cSCHeadChannel::getName()
{
    return m_sName;
}


QString &cSCHeadChannel::getAlias()
{
    return m_sAlias;
}


QString &cSCHeadChannel::getDescription()
{
    return m_sDescription;
}


bool cSCHeadChannel::isAvail()
{
    return m_bAvail;
}


QString cSCHeadChannel::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSCHeadChannel::m_ReadChannelStatus(QString &sInput)
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


