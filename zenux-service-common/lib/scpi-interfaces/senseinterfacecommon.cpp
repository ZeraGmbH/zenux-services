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
                                                                       .arg(channel->getName())
                                                                       .arg(channel->getDescription())
                                                                       .arg(port));
    }
    // additional we register measuring mode switch as resource
    QString measModeDescription = QString("Measuring mode switch %1").arg(getSenseModesSortedById().join(","));
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;MMODE;1;%1;%2;")
                                                                   .arg(measModeDescription)
                                                                   .arg(port));
}

QString SenseInterfaceCommon::scpiReadVersion(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_version;
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::m_ReadMModeCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return getSenseModesSortedById().join(";");
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString SenseInterfaceCommon::m_ReadSenseChannelCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return m_notifierSenseChannelCat.getString();
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
