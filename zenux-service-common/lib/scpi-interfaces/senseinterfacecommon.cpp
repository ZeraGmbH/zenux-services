#include "senseinterfacecommon.h"
#include "adjustmentflags.h"
#include "notzeronumgen.h"
#include "zscpi_response_definitions.h"
#include <i2cmultiplexerfactory.h>

QString SenseInterfaceCommon::m_version = "V1.00";

SenseInterfaceCommon::SenseInterfaceCommon(cSCPI *scpiInterface,
                                           I2cSettings *i2cSettings,
                                           SystemInfo *systemInfo,
                                           AbstractFactoryI2cCtrlPtr ctrlFactory) :
    cResource(scpiInterface),
    AdjustmentEeprom(i2cSettings->getDeviceNode(),
                     i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                     I2cMultiplexerFactory::createNullMuxer()),
    m_systemInfo(systemInfo),
    m_ctrlFactory(ctrlFactory)
{
}

SenseInterfaceCommon::~SenseInterfaceCommon()
{
    for(auto channel : qAsConst(m_channelList))
        delete channel;
    m_channelList.clear();
}

quint8 SenseInterfaceCommon::getAdjustmentStatus()
{
    quint8 adjustmentStatusMask = Adjustment::adjusted;
    // Loop adjustment state for all channels
    for(auto channel : qAsConst(m_channelList)) {
        quint8 channelFlags = channel->getAdjustmentStatus80Mask();
        // Currently there is one flag in channel flags only
        if((channelFlags & JustDataInterface::Justified)== 0) {
            adjustmentStatusMask = Adjustment::notAdjusted;
            break;
        }
    }
    // if we read wrong serial or version we are not adjusted in any case
    quint8 sernoVersionStatusMask = m_nSerialStatus;
    if (sernoVersionStatusMask != 0) {
        adjustmentStatusMask = Adjustment::notAdjusted;
        adjustmentStatusMask |= sernoVersionStatusMask;
    }
    return adjustmentStatusMask;
}

SenseChannelCommon *SenseInterfaceCommon::getChannel(QString &name)
{
    for(auto channel : qAsConst(m_channelList))
        if(channel->getName() == name)
            return channel;
    return nullptr;
}

QString SenseInterfaceCommon::getChannelByCtrlChannelNo(quint16 ctrlChannel)
{
    for(auto channel : qAsConst(m_channelList))
        if(channel->getCtrlChannel() == ctrlChannel)
            return channel->getName();
    return QString();
}

SenseRangeCommon *SenseInterfaceCommon::getRange(QString channelName, QString rangeName)
{
    SenseChannelCommon *channelFound = getChannel(channelName);
    if(channelFound)
        return channelFound->getRange(rangeName);
    return nullptr;
}

void SenseInterfaceCommon::computeSenseAdjData()
{
    for(auto channel : qAsConst(m_channelList))
        channel->computeJustData();
}

void SenseInterfaceCommon::registerResource(RMConnection *rmConnection, quint16 port)
{
    msgNrList.clear();
    for(auto channel : qAsConst(m_channelList)) {
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;%1;1;%2;%3;")
                                                                       .arg(channel->getName(), channel->getDescription())
                                                                       .arg(port));
    }
    // additional we register measuring mode switch as resource
    QString measModeDescription = QString("Measuring mode switch %1").arg(getSenseModesSortedById().join(","));
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;MMODE;1;%1;%2;")
                                                                   .arg(measModeDescription)
                                                                   .arg(port));
}

void SenseInterfaceCommon::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SENSE").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdVersion);
    addDelegate(QString("%1SENSE").arg(leadingNodes),"MMODE",SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SenseSystem::cmdMMode, &m_notifierSenseMMode);
    addDelegate(QString("%1SENSE:MMODE").arg(leadingNodes),"CATALOG",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdMModeCat);
    addDelegate(QString("%1SENSE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdChannelCat, &m_notifierSenseChannelCat);
    addDelegate(QString("%1SENSE:GROUP").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdGroupCat);
    addDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"INIT", SCPI::isCmd, m_pSCPIInterface, SenseSystem::initAdjData);
    addDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"COMPUTE", SCPI::isCmd, m_pSCPIInterface, SenseSystem::computeAdjData);
    for(auto channel : qAsConst(m_channelList)) {
        // we also must connect the signals for notification and for output
        connect(channel, &ScpiConnection::sendNotification, this, &ScpiConnection::sendNotification);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        connect(this, &ScpiConnection::removingSubscribers, channel, &ScpiConnection::onRemoveSubscribers);
        channel->initSCPIConnection(QString("%1SENSE").arg(leadingNodes));
    }
    QString cmdParent = QString("STATUS:PCB");
    addDelegate(cmdParent, "ADJUSTMENT", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdStatAdjustment);
}

void SenseInterfaceCommon::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseSystem::cmdVersion:
        protoCmd->m_sOutput = scpiReadVersion(protoCmd->m_sInput);
        break;
    case SenseSystem::cmdMMode:
        handleScpiReadWriteMMode(protoCmd);
        return;
    case SenseSystem::cmdMModeCat:
        protoCmd->m_sOutput = scpiReadMModeCatalog(protoCmd->m_sInput);
        break;
    case SenseSystem::cmdChannelCat:
        protoCmd->m_sOutput = scpiReadSenseChannelCatalog(protoCmd->m_sInput);
        break;
    case SenseSystem::cmdGroupCat:
        protoCmd->m_sOutput = scpiReadSenseGroupCatalog(protoCmd->m_sInput);
        break;
    case SenseSystem::initAdjData:
        protoCmd->m_sOutput = scpiInitSenseAdjDataAllChannelRanges(protoCmd->m_sInput);
        break;
    case SenseSystem::computeAdjData:
        protoCmd->m_sOutput = scpiComputeSenseAdjDataAllChannelRanges(protoCmd->m_sInput);
        break;
    case SenseSystem::cmdStatAdjustment:
        protoCmd->m_sOutput = scpiReadAdjStatus(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString SenseInterfaceCommon::scpiReadVersion(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_version;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiReadMModeCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return getSenseModesSortedById().join(";");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiReadSenseChannelCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_notifierSenseChannelCat.getString();
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiInitSenseAdjDataAllChannelRanges(QString &scpi)
{
    cSCPICommand cmd = scpi;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                for(auto channel : qAsConst(m_channelList))
                    channel->initJustData();
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiComputeSenseAdjDataAllChannelRanges(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if ( cmd.isCommand(1) && (cmd.getParam(0) == "") ) {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                computeSenseAdjData();
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::scpiReadAdjStatus(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return  QString("%1").arg(getAdjustmentStatus());
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void SenseInterfaceCommon::setNotifierSenseMMode()
{
    m_notifierSenseMMode = m_currSenseMode;
}

void SenseInterfaceCommon::setNotifierSenseChannelCat()
{
    // only prepared for !!! since we don't have hot plug for measuring channels yet
    int i;
    QString s;
    for (i = 0; i < m_channelList.count()-1; i++ )
        s += m_channelList.at(i)->getName() + ";";
    s += m_channelList.at(i)->getName();
    m_notifierSenseChannelCat = s;
}

QStringList SenseInterfaceCommon::getSenseModesSortedById()
{
    const QStringList modeNames = m_availSenseModesHash.keys();
    QMap<int, QString> sortedModes; // original COM implementation was sorted (MT hash random)
    for(const auto &modeName : modeNames)
        sortedModes[m_availSenseModesHash[modeName]] = modeName;
    return sortedModes.values();
}
