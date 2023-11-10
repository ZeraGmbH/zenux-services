#include "samplinginterface.h"
#include "scpiconnection.h"
#include "samplerange.h"
#include "atmel.h"
#include "samplingsettings.h"
#include "notzeronumgen.h"
#include "zscpi_response_definitions.h"

cSamplingInterface::cSamplingInterface(cSCPI *scpiInterface, SamplingSettings* samplingSettings) :
    cResource(scpiInterface)
{
    m_pllChannelList.append("0");
    m_pllChannelList.append("m0");
    m_pllChannelList.append("m1");
    m_pllChannelList.append("m2");
    m_pllChannelList.append("m3");
    m_pllChannelList.append("m4");
    m_pllChannelList.append("m5");

    QList<SamplingSettings::ChannelSettings*> channelSettings = samplingSettings->getChannelSettings();
    m_sName = "s0";
    m_sDescription = "Samplingsytem base frequency 10Hz..400Hz";
    m_sAlias = channelSettings.at(0)->m_sAlias;
    m_bAvail = channelSettings.at(0)->m_bAvail;
    m_sVersion = SamplingSystem::Version;
    m_nType = 0;

    m_SampleRangeList.append(new cSampleRange(m_pSCPIInterface, "F50Hz", 504, 0));
    m_SampleRangeList.append(new cSampleRange(m_pSCPIInterface, "F20Hz", 720, 1));

    Atmel::getInstance().setSamplingRange(0); // default we set 50Hz 504 samples
    setNotifierSampleChannelRange(); // we must intialize our setting (notifier)
}

void cSamplingInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SAMPLE").arg(leadingNodes),"VERSION", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdVersion);
    addDelegate(QString("%1SAMPLE").arg(leadingNodes),"SRATE", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdSampleRate);
    addDelegate(QString("%1SAMPLE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelCat);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelAlias);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"TYPE", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelType);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelStatus);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"RANGE", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SamplingSystem::cmdChannelRange, &notifierSampleChannelRange);
    addDelegate(QString("%1SAMPLE:%2:RANGE").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelRangeCat);
    addDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"PLL", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SamplingSystem::cmdPLL);
    addDelegate(QString("%1SAMPLE:%2:PLL").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdPLLCat);
    for(auto range : qAsConst(m_SampleRangeList)) {
        connect(range, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        range->initSCPIConnection(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName));
    }
}

void cSamplingInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SAMPLE;%1;1;%2;%3;").arg(m_sName).arg(m_sDescription).arg(port));
}

void cSamplingInterface::unregisterResource(RMConnection *rmConnection)
{
    unregister1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SAMPLE;%1;").arg(m_sName));
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
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cSamplingInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sVersion;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

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
                if ( Atmel::getInstance().setSamplingRange(m_SampleRangeList.at(i)->getSelCode()) == ZeraMControllerIo::cmddone) {
                    setNotifierSampleChannelRange();
                    return ZSCPI::scpiAnswer[ZSCPI::ack];
                }
                else
                    return ZSCPI::scpiAnswer[ZSCPI::errexec];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::nak];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::nak];
    }
}

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

QString cSamplingInterface::m_ReadWritePLL(QString &sInput)
{
    quint8 pll;
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (Atmel::getInstance().readPLLChannel(pll) == ZeraMControllerIo::cmddone)
            if (pll < m_pllChannelList.count())
                return m_pllChannelList.at(pll);
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else {
        if (cmd.isCommand(1)) {
            QString pllchn = cmd.getParam(0);
            pll = m_pllChannelList.indexOf(pllchn);
            if (Atmel::getInstance().setPLLChannel(pll) == ZeraMControllerIo::cmddone)
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

void cSamplingInterface::setNotifierSampleChannelRange()
{
    quint8 sRange;
    if (Atmel::getInstance().readSamplingRange(sRange) == ZeraMControllerIo::cmddone) {
        int i;
        for (i = 0; i < m_SampleRangeList.count(); i++)
            if (m_SampleRangeList.at(i)->getSelCode() == sRange)
                break;
        notifierSampleChannelRange = m_SampleRangeList.at(i)->getName();
    }
    else
        notifierSampleChannelRange = m_SampleRangeList.at(0)->getName();
}
