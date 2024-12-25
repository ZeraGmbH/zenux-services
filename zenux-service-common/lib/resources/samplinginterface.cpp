#include "samplinginterface.h"
#include "scpiconnection.h"
#include "samplerange.h"
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

    m_sName = "s0";
    m_sDescription = "Samplingsytem base frequency 10Hz..400Hz";
    m_sAlias = channelSettings.at(0)->m_sAlias;
    m_bAvail = channelSettings.at(0)->m_bAvail;
    m_sVersion = SamplingSystem::Version;
    m_nType = 0;

    // There is no change on sampling range supported: Can this go??
    m_SampleRangeList.append(new cSampleRange(m_scpiInterface, "F50Hz", 504, 0));
    m_SampleRangeList.append(new cSampleRange(m_scpiInterface, "F20Hz", 720, 1));
    setNotifierSampleChannelRange(); // we must intialize our setting (notifier)
}

void cSamplingInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SAMPLE").arg(leadingNodes),"VERSION", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdVersion);
    addDelegate(QString("%1SAMPLE").arg(leadingNodes),"SRATE", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdSampleRate);
    addDelegate(QString("%1SAMPLE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdChannelCat);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes, m_sName),"ALIAS", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdChannelAlias);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes, m_sName),"TYPE", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdChannelType);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes, m_sName),"STATUS", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdChannelStatus);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes, m_sName),"RANGE", SCPI::isQuery | SCPI::isCmdwP , m_scpiInterface, SamplingSystem::cmdChannelRange, &notifierSampleChannelRange);
    addDelegate(QString("%1SAMPLE:%2:RANGE").arg(leadingNodes, m_sName),"CATALOG", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdChannelRangeCat);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes, m_sName),"PLL", SCPI::isQuery | SCPI::isCmdwP , m_scpiInterface, SamplingSystem::cmdPLL);
    addDelegate(QString("%1SAMPLE:%2:PLL").arg(leadingNodes, m_sName),"CATALOG", SCPI::isQuery, m_scpiInterface, SamplingSystem::cmdPLLCat);
    for(auto range : qAsConst(m_SampleRangeList)) {
        connect(range, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        range->initSCPIConnection(QString("%1SAMPLE:%2").arg(leadingNodes, m_sName));
    }
}

void cSamplingInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SAMPLE;%1;1;%2;%3;").arg(m_sName, m_sDescription).arg(port));
}

void cSamplingInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SamplingSystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdSampleRate:
        protoCmd->m_sOutput = m_ReadSampleRate(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadSamplingChannelCatalog(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdChannelAlias:
        protoCmd->m_sOutput = m_ReadAlias(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdChannelType:
        protoCmd->m_sOutput = m_ReadType(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdChannelStatus:
        protoCmd->m_sOutput = m_ReadStatus(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdChannelRange:
        protoCmd->m_sOutput = m_ReadWriteSamplingRange(protoCmd->m_sInput);
        break;
    case SamplingSystem::cmdChannelRangeCat:
        protoCmd->m_sOutput = m_ReadSamplingRangeCatalog(protoCmd->m_sInput);
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

QString cSamplingInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sVersion;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString cSamplingInterface::m_ReadSamplingChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sName; // we only have 1 channel
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString cSamplingInterface::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_sAlias);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString cSamplingInterface::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nType);
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString cSamplingInterface::m_ReadStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint32 r = (m_bAvail) ? 0 : 1 << 31;
        return QString("%1").arg(r);
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

// Is this art or can we litter it?
QString cSamplingInterface::m_ReadWriteSamplingRange(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return notifierSampleChannelRange.getString();
    else {
        if (cmd.isCommand(1)) {
            QString srng = cmd.getParam(0);
            int i;
            for (i = 0; i < m_SampleRangeList.count(); i++)
                if (m_SampleRangeList.at(i)->getName() == srng)
                    break;
            if (i < m_SampleRangeList.count()) {
                setNotifierSampleChannelRange();
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::nak];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::nak];
    }
}

// And another: Is this art or can we litter it?
QString cSamplingInterface::m_ReadSampleRate(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QString s = notifierSampleChannelRange.getString(); // our actual sample channels range
        for(int i = 0; i < m_SampleRangeList.count(); i++) {
            if (m_SampleRangeList.at(i)->getName() == s)
                return QString("%1").arg(m_SampleRangeList.at(i)->getSRate());
        }
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

// This lies and is there for compatibility?
QString cSamplingInterface::m_ReadSamplingRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QStringList rangeNames;
        for(const auto &samplerange : qAsConst(m_SampleRangeList))
            rangeNames.append(samplerange->getName());
        return rangeNames.join(";");
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

// Looks odd but this code is a leftover and we are not yet prepared to remove
// it completely. Background: Commands to change sampling range were just noop
// most likely inherited from WM3000 where we had different samling frequencies
// But who knows...
void cSamplingInterface::setNotifierSampleChannelRange()
{
    quint8 sRange = 0;
    for (int i = 0; i < m_SampleRangeList.count(); i++) {
        if (m_SampleRangeList.at(i)->getSelCode() == sRange) {
            notifierSampleChannelRange = m_SampleRangeList.at(i)->getName();
            return;
        }
    }
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

