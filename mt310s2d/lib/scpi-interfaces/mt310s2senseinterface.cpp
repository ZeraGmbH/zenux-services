#include "mt310s2senseinterface.h"
#include "mt310s2dglobal.h"
#include "mt310s2sensechannel.h"
#include "mt310s2senserange.h"
#include "zscpi_response_definitions.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include <xmlsettings.h>
#include <QStringList>
#include <QDomDocument>
#include <QDateTime>
#include <QDebug>
#include <QFile>

const QString sVoltageChannelDescription = "Measuring channel 0..250V 50Hz/150kHz";
const QString sCurrentChannelDescription = "Measuring channel 0..1000A 50Hz/150kHz";

Mt310s2SenseInterface::Mt310s2SenseInterface(std::shared_ptr<cSCPI> scpiInterface,
                                             I2cSettings* i2cSettings,
                                             cSenseSettings* senseSettings,
                                             SystemInfo *systemInfo,
                                             AbstractFactoryI2cCtrlPtr ctrlFactory) :
    SenseInterfaceCommon(scpiInterface,
                           i2cSettings,
                           systemInfo,
                           ctrlFactory,
                           QHash<QString, int>{{"AC", modeAC}, {"HF", modeHF}, {"ADJ", modeADJ}})
{
    m_channelList = setChannelAndRanges(senseSettings,
                                        m_adjData,
                                        m_scpiInterface,
                                        m_ctrlFactory);
    injectAdjToChannelRanges();
    setSenseMode("AC");
    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet
}

QList<SenseChannelCommon*> Mt310s2SenseInterface::setChannelAndRanges(cSenseSettings* senseSettings,
                                                                       AdjDataPtr adjData,
                                                                       std::shared_ptr<cSCPI> scpi,
                                                                       AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    constexpr quint16 rangeFlagsDevice = modeAC | modeADJ | Direct;
    constexpr quint16 rangeFlagsIntern = modeADJ | Direct;

    QList<SenseSystem::cChannelSettings*> channelSettings = senseSettings->getChannelSettings();
    QList<SenseChannelCommon*> channels;
    channels.append(new Mt310s2SenseChannel(scpi, sVoltageChannelDescription,"V", channelSettings.at(0), 0, ctrlFactory));
    channels.append(new Mt310s2SenseChannel(scpi, sVoltageChannelDescription,"V", channelSettings.at(1), 1, ctrlFactory));
    channels.append(new Mt310s2SenseChannel(scpi, sVoltageChannelDescription,"V", channelSettings.at(2), 2, ctrlFactory));
    channels.append(new Mt310s2SenseChannel(scpi, sVoltageChannelDescription,"V", channelSettings.at(6), 6, ctrlFactory));
    channels.append(new Mt310s2SenseChannel(scpi, sCurrentChannelDescription,"A", channelSettings.at(3), 3, ctrlFactory));
    channels.append(new Mt310s2SenseChannel(scpi, sCurrentChannelDescription,"A", channelSettings.at(4), 4, ctrlFactory));
    channels.append(new Mt310s2SenseChannel(scpi, sCurrentChannelDescription,"A", channelSettings.at(5), 5, ctrlFactory));
    channels.append(new Mt310s2SenseChannel(scpi, sCurrentChannelDescription,"A", channelSettings.at(7), 7, ctrlFactory));

    for (int i = 0; i < 4; i++) {
        QList<SenseRangeCommon*> rngListUAll;

        // TODO:
        // * Do checks isInvalidAdjDataOrChannelRangeAvail only on ranges introduced later

        QString rangeName = "250V";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListUAll.append(new Mt310s2SenseRange(scpi, rangeName, true, 250.0, 4415057.0, 5518821.0, 0, rangeFlagsDevice));
        rangeName = "8V";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListUAll.append(new Mt310s2SenseRange(scpi, rangeName, true,   8.0, 3355443.0, 4194304.0, 1, rangeFlagsDevice));
        rangeName = "100mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListUAll.append(new Mt310s2SenseRange(scpi, rangeName, true,   0.1, 4026532.0, 5033165.0, 2, rangeFlagsDevice));

        channels.at(i)->setRangeList(rngListUAll);
    }

    for (int i = 4; i < 7; i++) {
        QList<SenseRangeCommon*> rngListIExceptAux;

        QString rangeName = "10A";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true,  10.0, 3197613.0, 3997016.0,  0, rangeFlagsDevice));
        rangeName = "5A";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true,   5.0, 3197613.0, 3997016.0,  1, rangeFlagsDevice));
        rangeName = "2.5A";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true,   2.5, 3997016.0, 4996270.0,  2, rangeFlagsDevice));
        rangeName = "1.0A";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true,   1.0, 4177527.0, 5221909.0,  3, rangeFlagsDevice));
        rangeName = "500mA";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true,   0.5, 4177527.0, 5221909.0,  4, rangeFlagsDevice));
        rangeName = "250mA";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true,  0.25, 4177527.0, 5221909.0,  5, rangeFlagsDevice));
        rangeName = "100mA";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true,   0.1, 4177527.0, 5221909.0,  6, rangeFlagsDevice));
        rangeName = "50mA";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true,  0.05, 4177527.0, 5221909.0,  7, rangeFlagsDevice));
        rangeName = "25mA";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, true, 0.025, 4177527.0, 5221909.0,  8, rangeFlagsDevice));

        rangeName = "8V";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  8.0, 3355443.0, 4194304.0,  9, rangeFlagsIntern));
        rangeName = "5V";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  5.0, 4194304.0, 5242880.0, 10, rangeFlagsIntern));
        rangeName = "2V";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  2.0, 2835586.0, 3544483.0, 11, rangeFlagsIntern));
        rangeName = "1V";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  1.0, 2835586.0, 3544483.0, 12, rangeFlagsIntern));
        rangeName = "500mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  0.5, 3544482.0, 4430603.0, 13, rangeFlagsIntern));
        rangeName = "200mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  0.2, 2684355.0, 3355444.0, 14, rangeFlagsIntern));
        rangeName = "100mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  0.1, 3355443.0, 4194304.0, 15, rangeFlagsIntern));
        rangeName = "50mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false, 0.05, 3355443.0, 4194304.0, 16, rangeFlagsIntern));
        rangeName = "20mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false, 0.02, 2684355.0, 3355444.0, 17, rangeFlagsIntern));
        rangeName = "10mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false, 0.01, 3355443.0, 4194304.0, 18, rangeFlagsIntern));
        rangeName = "5mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,0.005, 3355443.0, 4194304.0, 19, rangeFlagsIntern));
        rangeName = "2mV";
        if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(i)->getName(), rangeName))
            rngListIExceptAux.append(new Mt310s2SenseRange(scpi, rangeName, false,0.002, 2684355.0, 3355444.0, 20, rangeFlagsIntern));

        channels.at(i)->setRangeList(rngListIExceptAux);
    }

    // IAUX
    constexpr int phase = 7;
    QList<SenseRangeCommon*> rngListIAux;
    // rValue = 1e-12 (yes a pico Ampere error - for 100% range sample value):
    // We did not yet find the place but it seems 0.0 turns into a divisor causing DSP to run almost infinite
    // loop on range change when running power1module on a channnel supporting 0A
    rngListIAux.append(new Mt310s2SenseRange(scpi, SenseRangeCommon::rangeNameCurrentNull, true,    1e-12, 3197613.0, 3997016.0,  0, rangeFlagsDevice));
    QString rangeName = "8V";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,   8.0, 3355443.0, 4194304.0,  9, rangeFlagsIntern));
    rangeName = "5V";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,   5.0, 4194304.0, 5242880.0, 10, rangeFlagsIntern));
    rangeName = "2V";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,   2.0, 2835586.0, 3544483.0, 11, rangeFlagsIntern));
    rangeName = "1V";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,   1.0, 2835586.0, 3544483.0, 12, rangeFlagsIntern));
    rangeName = "500mV";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,   0.5, 3544482.0, 4430603.0, 13, rangeFlagsIntern));
    rangeName = "200mV";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,   0.2, 2684355.0, 3355444.0, 14, rangeFlagsIntern));
    rangeName = "100mV";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,   0.1, 3355443.0, 4194304.0, 15, rangeFlagsIntern));
    rangeName = "50mV";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  0.05, 3355443.0, 4194304.0, 16, rangeFlagsIntern));
    rangeName = "20mV";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  0.02, 2684355.0, 3355444.0, 17, rangeFlagsIntern));
    rangeName = "10mV";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false,  0.01, 3355443.0, 4194304.0, 18, rangeFlagsIntern));
    rangeName = "5mV";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false, 0.005, 3355443.0, 4194304.0, 19, rangeFlagsIntern));
    rangeName = "2mV";
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channels.at(phase)->getName(), rangeName))
        rngListIAux.append(new Mt310s2SenseRange(scpi, rangeName, false, 0.002, 2684355.0, 3355444.0, 20, rangeFlagsIntern));

    channels.at(7)->setRangeList(rngListIAux);
    return channels;
}


QString Mt310s2SenseInterface::getPcbName()
{
    return LeiterkartenName;
}

QString Mt310s2SenseInterface::getXmlType()
{
    return "PCBAdjustmentData";
}

bool Mt310s2SenseInterface::isRangePartOfAdjXmlExport(SenseRangeCommon *range)
{
    return range->getMMask() & Direct;
}

const char *Mt310s2SenseInterface::getAdjExportedVersion()
{
    return ServerVersion;
}

QString Mt310s2SenseInterface::scpiReadSenseGroupCatalog(QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return ";"; // this server has no grouping constraints
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

bool Mt310s2SenseInterface::setSenseMode(QString sMode)
{
    if (m_availSenseModesHash.contains(sMode)) {
        quint8 mode = m_availSenseModesHash[sMode];
        for(auto channel : qAsConst(m_channelList))
            channel->setMMode(mode);
        m_currSenseMode = sMode;
        setNotifierSenseMMode();
        return true;
    }
    return false;
}
