#include "samplinginterface.h"
#include "samplingsettings.h"
#include "notzeronumgen.h"
#include "zscpi_response_definitions.h"

cSamplingInterface::cSamplingInterface(std::shared_ptr<cSCPI> scpiInterface,
                                       SamplingSettings* samplingSettings,
                                       AbstractFactoryI2cCtrlPtr ctrlFactory) :
    cResource(scpiInterface),
    m_ctrlFactory(ctrlFactory)
{
    QList<SamplingSettings::ChannelSettings*> channelSettings = samplingSettings->getChannelSettings();
    for (const QString &pllChannel : qAsConst(channelSettings.at(0)->m_pllChannels)) {
        m_pllChannelList.append(pllChannel);
    }

    m_sDescription = "Samplingsytem base frequency 10Hz..400Hz";
    m_bAvail = channelSettings.at(0)->m_bAvail;
}

void cSamplingInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SAMPLE").arg(leadingNodes),"SRATE", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdSampleRate);
    addDelegate(QString("%1SAMPLE:S0").arg(leadingNodes),"PLL", SCPI::isQuery | SCPI::isCmdwP , m_scpiInterface, SamplingSystem::cmdPLL);
    addDelegate(QString("%1SAMPLE:S0:PLL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdPLLCat);
}

void cSamplingInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SAMPLE;S0;1;%2;%3;").arg(m_sDescription).arg(port));
}

void cSamplingInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SamplingSystem::cmdSampleRate:
        protoCmd->m_sOutput = m_ReadSampleRate(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdPLL:
        protoCmd->m_sOutput = m_ReadWritePLL(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdPLLCat:
        protoCmd->m_sOutput = m_ReadPLLCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput) {
        emit cmdExecutionDone(protoCmd);
    }
}

// Why do we still ask pcb-server for samplereate?
QString cSamplingInterface::m_ReadSampleRate(QString &sInput)
{
    Q_UNUSED(sInput)
    return "504";
}

QString cSamplingInterface::m_ReadWritePLL(QString &sInput)
{
    quint8 pll;
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_ctrlFactory->getPllController()->readPLLChannel(pll) == ZeraMControllerIo::cmddone)
            if (pll < m_pllChannelList.count())
                return m_pllChannelList.at(pll);
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else {
        if (cmd.isCommand(1)) {
            QString pllchn = cmd.getParam(0);
            pll = m_pllChannelList.indexOf(pllchn);
            if (m_ctrlFactory->getPllController()->setPLLChannel(pll) == ZeraMControllerIo::cmddone)
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            else
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
        return ZSCPI::scpiAnswer[ZSCPI::nak];
    }
}

QString cSamplingInterface::m_ReadPLLCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_pllChannelList.join(";");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

