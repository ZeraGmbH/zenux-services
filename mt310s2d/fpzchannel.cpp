#include <scpiconnection.h>
#include "fpzchannel.h"
#include <protonetcommand.h>
#include "settings/sourcesettings.h"
#include <scpi.h>

cFPZChannel::cFPZChannel(cSCPI *scpiinterface, QString description, quint8 nr, SourceSystem::cChannelSettings *cSettings) :
    cSCPIConnection(scpiinterface),
    m_sDescription(description)
{
    m_sName = QString("fo%1").arg(nr);
    m_sAlias = cSettings->m_sAlias;
    m_nDspServer = cSettings->m_nDspServerPort;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_nType = 0;
    m_fFormFactor = FPZChannel::FormFactor;
    initNotifier(notifierConstant); // we hold the constant as a notifier
    initNotifier(notifierPowerType);
    m_bAvail = cSettings->avail;
}


void cFPZChannel::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, FPZChannel::cmdAlias);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"TYPE", SCPI::isQuery, m_pSCPIInterface, FPZChannel::cmdType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"DSPSERVER", SCPI::isQuery, m_pSCPIInterface, FPZChannel::cmdDspServer);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"DSPCHANNEL", SCPI::isQuery, m_pSCPIInterface, FPZChannel::cmdDspChannel);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, FPZChannel::cmdStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"FFACTOR", SCPI::isQuery, m_pSCPIInterface, FPZChannel::cmdFormFactor);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"CONSTANT", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, FPZChannel::cmdConstant);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"POWTYPE", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, FPZChannel::cmdPowtype);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
}


void cFPZChannel::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case FPZChannel::cmdAlias:
        protoCmd->m_sOutput = m_ReadAlias(protoCmd->m_sInput);
        break;
    case FPZChannel::cmdType:
        protoCmd->m_sOutput = m_ReadType(protoCmd->m_sInput);
        break;
    case FPZChannel::cmdDspServer:
        protoCmd->m_sOutput = m_ReadDspServer(protoCmd->m_sInput);
        break;
    case FPZChannel::cmdDspChannel:
        protoCmd->m_sOutput = m_ReadDspChannel(protoCmd->m_sInput);
        break;
    case FPZChannel::cmdStatus:
        protoCmd->m_sOutput = m_ReadChannelStatus(protoCmd->m_sInput);
        break;
    case FPZChannel::cmdFormFactor:
        protoCmd->m_sOutput = m_ReadFFactor(protoCmd->m_sInput);
        break;
    case FPZChannel::cmdConstant:
        protoCmd->m_sOutput = m_ReadWriteConstant(protoCmd->m_sInput);
        break;
    case FPZChannel::cmdPowtype:
        protoCmd->m_sOutput = m_ReadWritePowerType(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


void cFPZChannel::initNotifier(cNotificationString& notifier)
{
    notifier = "0.0";
}


QString &cFPZChannel::getName()
{
    return m_sName;
}


QString &cFPZChannel::getAlias()
{
    return m_sAlias;
}


QString &cFPZChannel::getDescription()
{
    return m_sDescription;
}


bool cFPZChannel::isAvail()
{
    return m_bAvail;
}


QString cFPZChannel::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nType);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadDspServer(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nDspServer);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadChannelStatus(QString &sInput)
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


QString cFPZChannel::m_ReadFFactor(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return QString("%1").arg(m_fFormFactor);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadWriteConstant(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())

    {
        emit strNotifier(&notifierConstant);
        return notifierConstant.getString();
    }
    else
        if (cmd.isCommand(1))
        {
            QString constant = cmd.getParam(0);
            notifierConstant = constant;
            return SCPI::scpiAnswer[SCPI::ack];
        }
        else
            return SCPI::scpiAnswer[SCPI::nak];
}


QString cFPZChannel::m_ReadWritePowerType(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())

    {
        emit strNotifier(&notifierPowerType);
        return notifierPowerType.getString();
    }
    else
        if (cmd.isCommand(1))
        {
            QString powertype = cmd.getParam(0);
            notifierPowerType = powertype;
            return SCPI::scpiAnswer[SCPI::ack];
        }
        else
            return SCPI::scpiAnswer[SCPI::nak];
}
