#include "samplinginterface.h"
#include "samplingsettings.h"
#include "notzeronumgen.h"
#include "zscpi_response_definitions.h"

enum Commands
{
    cmdSampleRate,
    cmdPLL,
    cmdPLLCat
};

cSamplingInterface::cSamplingInterface(std::shared_ptr<cSCPI> scpiInterface,
                                       SamplingSettingsPtr samplingSettings,
                                       AbstractFactoryI2cCtrlPtr ctrlFactory) :
    cResource(scpiInterface),
    m_ctrlFactory(ctrlFactory)
{
    const QList<SamplingSettings::ChannelSettings*> &channelSettings = samplingSettings->getChannelSettings();
    const QStringList &pllChannels = channelSettings.at(0)->m_pllChannels;
    for (const QString &pllChannel : pllChannels)
        m_pllChannelList.append(pllChannel);

    m_sDescription = "Samplingsytem base frequency 10Hz..400Hz";
    m_bAvail = channelSettings.at(0)->m_bAvail;
}

void cSamplingInterface::initSCPIConnection()
{
    addDelegate("SAMPLE", "SRATE", SCPI::isQuery, m_scpiInterface, cmdSampleRate);
    addDelegate("SAMPLE:S0", "PLL", SCPI::isQuery | SCPI::isCmdwP , m_scpiInterface, cmdPLL);
    addDelegate("SAMPLE:S0:PLL", "CATALOG", SCPI::isQuery, m_scpiInterface, cmdPLLCat);
}

void cSamplingInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SAMPLE;S0;1;%2;%3;").arg(m_sDescription).arg(port));
}

void cSamplingInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode)
    {
    case cmdSampleRate:
        protoCmd->m_sOutput = scpiReadSampleRate(protoCmd->m_sInput);
        break;
    case cmdPLL:
        protoCmd->m_sOutput = scpiReadWritePLL(protoCmd->m_sInput);
        break;
    case cmdPLLCat:
        protoCmd->m_sOutput = scpiReadPLLCatalog(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput) {
        emit cmdExecutionDone(protoCmd);
    }
}

// Why do we still ask pcb-server for samplereate?
QString cSamplingInterface::scpiReadSampleRate(const QString &scpi)
{
    Q_UNUSED(scpi)
    return "504";
}

QString cSamplingInterface::scpiReadWritePLL(const QString &scpi)
{
    quint8 pll;
    cSCPICommand cmd = scpi;
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

QString cSamplingInterface::scpiReadPLLCatalog(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_pllChannelList.join(";");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

