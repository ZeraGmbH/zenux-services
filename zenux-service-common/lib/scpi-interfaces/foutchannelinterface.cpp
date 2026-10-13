#include "foutchannelinterface.h"
#include "zscpi_response_definitions.h"

enum Commands
{
    cmdAlias,
    cmdType,
    cmdDspServer,
    cmdDspChannel,
    cmdStatus,
    cmdFormFactor,
};

static constexpr double FormFactor = 5.6294995e6; // fout = (Pact/Pnenn) * FPZnenn * FormFactor

FOutChannelInterface::FOutChannelInterface(const std::shared_ptr<cSCPI> &scpiInterface,
                                           const QString &description,
                                           quint8 nr,
                                           FOutSettings::ChannelSettings *cSettings) :
    ScpiConnection(scpiInterface),
    m_sName(QString("fo%1").arg(nr)),
    m_sAlias(cSettings->m_sAlias),
    m_sDescription(description),
    m_nDspServer(cSettings->m_nDspServerPort),
    m_nDspChannel(cSettings->m_nDspChannel),
    m_fFormFactor(FormFactor),
    m_bAvail(cSettings->avail)
{
}

void FOutChannelInterface::initSCPIConnection(const QString &leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_sName),"ALIAS", SCPI::isQuery, m_scpiInterface, cmdAlias);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_sName),"TYPE", SCPI::isQuery, m_scpiInterface, cmdType);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_sName),"DSPSERVER", SCPI::isQuery, m_scpiInterface, cmdDspServer);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_sName),"DSPCHANNEL", SCPI::isQuery, m_scpiInterface, cmdDspChannel);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_sName),"STATUS", SCPI::isQuery, m_scpiInterface, cmdStatus);
    addDelegate(QString("%1%2").arg(adjLeadNodes, m_sName),"FFACTOR", SCPI::isQuery, m_scpiInterface, cmdFormFactor);
}

void FOutChannelInterface::executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd)
{
    switch (cmdCode)
    {
    case cmdAlias:
        protoCmd->m_sOutput = scpiReadAlias(protoCmd->m_sInput);
        break;
    case cmdType:
        protoCmd->m_sOutput = scpiReadType(protoCmd->m_sInput);
        break;
    case cmdDspServer:
        protoCmd->m_sOutput = scpiReadDspServer(protoCmd->m_sInput);
        break;
    case cmdDspChannel:
        protoCmd->m_sOutput = scpiReadDspChannel(protoCmd->m_sInput);
        break;
    case cmdStatus:
        protoCmd->m_sOutput = scpiReadChannelStatus(protoCmd->m_sInput);
        break;
    case cmdFormFactor:
        protoCmd->m_sOutput = scpiReadFFactor(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

const QString &FOutChannelInterface::getName()
{
    return m_sName;
}

const QString &FOutChannelInterface::getAlias()
{
    return m_sAlias;
}

const QString &FOutChannelInterface::getDescription()
{
    return m_sDescription;
}

bool FOutChannelInterface::isAvail()
{
    return m_bAvail;
}

QString FOutChannelInterface::scpiReadAlias(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_sAlias;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString FOutChannelInterface::scpiReadType(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_nType);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString FOutChannelInterface::scpiReadDspServer(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_nDspServer);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString FOutChannelInterface::scpiReadDspChannel(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_nDspChannel);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString FOutChannelInterface::scpiReadChannelStatus(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        quint32 r;
        r = ((m_bAvail) ? 0 : 1 << 31);
            return QString("%1").arg(r);
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString FOutChannelInterface::scpiReadFFactor(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_fFormFactor);
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
