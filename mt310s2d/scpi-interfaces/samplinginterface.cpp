#include "samplinginterface.h"
#include <protonetcommand.h>
#include <scpiconnection.h>
#include "samplerange.h"
#include "mt310s2d.h"
#include "micro-controller-io/atmel.h"
#include "settings/samplingsettings.h"

cSamplingInterface::cSamplingInterface(cMT310S2dServer* server) :
    cResource(server->getSCPIInterface()),
    m_pMyServer(server)
{
    m_pllChannelList.append("m0");
    m_pllChannelList.append("m1");
    m_pllChannelList.append("m2");
    m_pllChannelList.append("m6");
    m_pllChannelList.append("m3");
    m_pllChannelList.append("m4");
    m_pllChannelList.append("m5");
    m_pllChannelList.append("m7");

    QList<SamplingSystem::cChannelSettings*> channelSettings;
    channelSettings = m_pMyServer->m_pSamplingSettings->getChannelSettings();

    m_sName = "s0";
    m_sDescription = "Samplingsytem base frequency 10Hz..400Hz";
    m_sAlias = channelSettings.at(0)->m_sAlias;
    m_bAvail = channelSettings.at(0)->m_bAvail;
    m_sVersion = SamplingSystem::Version;
    m_nType = 0;

    m_SampleRangeList.append(new cSampleRange(m_pMyServer->getSCPIInterface(),"F50Hz", 504, 0));
    m_SampleRangeList.append(new cSampleRange(m_pMyServer->getSCPIInterface(),"F20Hz", 720, 1));

    pAtmel->setSamplingRange(0); // default we set 50Hz 504 samples
    setNotifierSampleChannelRange(); // we must intialize our setting (notifier)
}

void cSamplingInterface::initSCPIConnection(QString leadingNodes)
{
    if (leadingNodes != "")
        leadingNodes += ":";
    cSCPIDelegate* delegate;
    delegate = new cSCPIDelegate(QString("%1SAMPLE").arg(leadingNodes),"VERSION", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE").arg(leadingNodes),"SRATE", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdSampleRate);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"ALIAS", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelAlias);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"TYPE", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelType);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"STATUS", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"RANGE", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SamplingSystem::cmdChannelRange);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2:RANGE").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdChannelRangeCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName),"PLL", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SamplingSystem::cmdPLL);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SAMPLE:%2:PLL").arg(leadingNodes).arg(m_sName),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SamplingSystem::cmdPLLCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    for(auto range : m_SampleRangeList) {
        connect(range, SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
        range->initSCPIConnection(QString("%1SAMPLE:%2").arg(leadingNodes).arg(m_sName));
    }
}

void cSamplingInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SAMPLE;%1;1;%2;%3;").arg(m_sName).arg(m_sDescription).arg(port));
}

void cSamplingInterface::unregisterResource(cRMConnection *rmConnection)
{
    unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SAMPLE;%1;").arg(m_sName));
}

void cSamplingInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
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
    if (cmd.isQuery()) {
        return m_sVersion;
    }
    return SCPI::scpiAnswer[SCPI::nak];
}

QString cSamplingInterface::m_ReadSampleRate(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QString s = notifierSampleChannelRange.getString(); // our actual sample channels range
        for(int i = 0; i < m_SampleRangeList.count(); i++) {
            if (m_SampleRangeList.at(i)->getName() == s) {
                return QString("%1").arg(m_SampleRangeList.at(i)->getSRate());
            }
        }
    }
    return SCPI::scpiAnswer[SCPI::nak];
}

QString cSamplingInterface::m_ReadSamplingChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sName; // we only have 1 channel
    return SCPI::scpiAnswer[SCPI::nak];
}

QString cSamplingInterface::m_ReadAlias(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_sAlias);
    return SCPI::scpiAnswer[SCPI::nak];
}

QString cSamplingInterface::m_ReadType(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return QString("%1").arg(m_nType);
    }
    return SCPI::scpiAnswer[SCPI::nak];
}


QString cSamplingInterface::m_ReadStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        quint32 r = (m_bAvail) ? 0 : 1 << 31;
        return QString("%1").arg(r);
    }
    return SCPI::scpiAnswer[SCPI::nak];
}

QString cSamplingInterface::m_ReadWriteSamplingRange(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        emit strNotifier(&notifierSampleChannelRange); // we need this in case that the client wants notification
        return notifierSampleChannelRange.getString();
    }
    else {
        if (cmd.isCommand(1)) {
            QString srng = cmd.getParam(0);
            int i;
            for  (i = 0; i < m_SampleRangeList.count(); i++) {
                if (m_SampleRangeList.at(i)->getName() == srng) {
                    break;
                }
            }
            if (i < m_SampleRangeList.count()) {
                if ( pAtmel->setSamplingRange(m_SampleRangeList.at(i)->getSelCode()) == ZeraMcontrollerBase::cmddone) {
                    setNotifierSampleChannelRange();
                    return SCPI::scpiAnswer[SCPI::ack];
                }
                else {
                    return SCPI::scpiAnswer[SCPI::errexec];
                }
            }
            else {
                return SCPI::scpiAnswer[SCPI::nak];
            }
        }
        else {
            return SCPI::scpiAnswer[SCPI::nak];
        }
    }
}


QString cSamplingInterface::m_ReadSamplingRangeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        int i, n;
        QString s;

        n = m_SampleRangeList.count();
        s = m_SampleRangeList.at(0)->getName();

        if (n > 1)
            for (i = 1; i < n; i++)
                s = s + ";" + m_SampleRangeList.at(i)->getName();

        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSamplingInterface::m_ReadWritePLL(QString &sInput)
{
    quint8 pll;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (pAtmel->readPLLChannel(pll) == ZeraMcontrollerBase::cmddone)
        {
            if (pll < 8) // then everything is ok
                return m_pllChannelList.at(pll);
        }

        return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString pllchn = cmd.getParam(0);
            pll = m_pllChannelList.indexOf(pllchn);

            if (pAtmel->setPLLChannel(pll) == ZeraMcontrollerBase::cmddone)
                return SCPI::scpiAnswer[SCPI::ack];
            else
                return SCPI::scpiAnswer[SCPI::errexec];
        }

        return SCPI::scpiAnswer[SCPI::nak];
    }
}


QString cSamplingInterface::m_ReadPLLCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return m_pllChannelList.join(";");
    }
    return SCPI::scpiAnswer[SCPI::nak];
}


void cSamplingInterface::setNotifierSampleChannelRange()
{
    int i;
    quint8 sRange;

    if (pAtmel->readSamplingRange(sRange) == ZeraMcontrollerBase::cmddone)
    {
        for (i = 0; i < m_SampleRangeList.count(); i++)
            if (m_SampleRangeList.at(i)->getSelCode() == sRange)
                break;
        notifierSampleChannelRange = m_SampleRangeList.at(i)->getName();
    }
    else
        notifierSampleChannelRange = m_SampleRangeList.at(0)->getName();
}





