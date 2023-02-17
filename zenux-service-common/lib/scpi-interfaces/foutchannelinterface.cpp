#include "foutchannelinterface.h"

enum Commands
{
    cmdAlias,
    cmdType,
    cmdDspServer,
    cmdDspChannel,
    cmdStatus,
    cmdFormFactor,
    cmdConstant,
    cmdPowtype
};

static constexpr double FormFactor = 5.6294995e6; // fout = (Pact/Pnenn) * FPZnenn * FormFactor

FOutChannelInterface::FOutChannelInterface(cSCPI *scpiinterface, QString description, quint8 nr, FOutSettings::ChannelSettings *cSettings) :
    ScpiConnection(scpiinterface),
    m_sDescription(description)
{
    m_sName = QString("fo%1").arg(nr);
    m_sAlias = cSettings->m_sAlias;
    m_nDspServer = cSettings->m_nDspServerPort;
    m_nDspChannel = cSettings->m_nDspChannel;
    m_nType = 0;
    m_fFormFactor = FormFactor;
    initNotifier(notifierConstant); // we hold the constant as a notifier
    initNotifier(notifierPowerType);
    m_bAvail = cSettings->avail;
}

void FOutChannelInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"TYPE", SCPI::isQuery, m_pSCPIInterface, cmdType);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"DSPSERVER", SCPI::isQuery, m_pSCPIInterface, cmdDspServer);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"DSPCHANNEL", SCPI::isQuery, m_pSCPIInterface, cmdDspChannel);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, cmdStatus);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"FFACTOR", SCPI::isQuery, m_pSCPIInterface, cmdFormFactor);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"CONSTANT", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, cmdConstant,&notifierConstant);
    addDelegate(QString("%1%2").arg(leadingNodes).arg(m_sName),"POWTYPE", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, cmdPowtype, &notifierPowerType);
}

void FOutChannelInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case cmdAlias:
        protoCmd->m_sOutput = readAlias(protoCmd->m_sInput);
        break;
    case cmdType:
        protoCmd->m_sOutput = readType(protoCmd->m_sInput);
        break;
    case cmdDspServer:
        protoCmd->m_sOutput = readDspServer(protoCmd->m_sInput);
        break;
    case cmdDspChannel:
        protoCmd->m_sOutput = readDspChannel(protoCmd->m_sInput);
        break;
    case cmdStatus:
        protoCmd->m_sOutput = readChannelStatus(protoCmd->m_sInput);
        break;
    case cmdFormFactor:
        protoCmd->m_sOutput = readFFactor(protoCmd->m_sInput);
        break;
    case cmdConstant:
        protoCmd->m_sOutput = readWriteConstant(protoCmd->m_sInput);
        break;
    case cmdPowtype:
        protoCmd->m_sOutput = readWritePowerType(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

void FOutChannelInterface::initNotifier(NotificationString &notifier)
{
    notifier = "0.0";
}

QString &FOutChannelInterface::getName()
{
    return m_sName;
}

QString &FOutChannelInterface::getAlias()
{
    return m_sAlias;
}

QString &FOutChannelInterface::getDescription()
{
    return m_sDescription;
}

bool FOutChannelInterface::isAvail()
{
    return m_bAvail;
}

QString FOutChannelInterface::readAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FOutChannelInterface::readType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nType);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FOutChannelInterface::readDspServer(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nDspServer);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FOutChannelInterface::readDspChannel(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FOutChannelInterface::readChannelStatus(QString &sInput)
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

QString FOutChannelInterface::readFFactor(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_fFormFactor);
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FOutChannelInterface::readWriteConstant(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return notifierConstant.getString();
    }
    else if (cmd.isCommand(1)) {
        QString constant = cmd.getParam(0);
        notifierConstant = constant;
        return SCPI::scpiAnswer[SCPI::ack];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString FOutChannelInterface::readWritePowerType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return notifierPowerType.getString();
    }
    else if (cmd.isCommand(1)) {
        QString powertype = cmd.getParam(0);
        notifierPowerType = powertype;
        return SCPI::scpiAnswer[SCPI::ack];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}
