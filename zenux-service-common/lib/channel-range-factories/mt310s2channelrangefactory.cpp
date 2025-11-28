#include "mt310s2channelrangefactory.h"
#include "senseinterfacecommon.h"
#include "mtxxxs2sensechannel.h"
#include <cmath>

QList<SenseChannelCommon *> MT310s2ChannelRangeFactory::createChannelAndRanges(cSenseSettingsPtr senseSettings,
                                                                               AdjDataPtr adjData,
                                                                               std::shared_ptr<cSCPI> scpi,
                                                                               AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    qInfo("Set channels/ranges for MT310s2");
    constexpr quint16 rangeFlagsDevice = modeAC | modeADJ | Direct;
    constexpr quint16 rangeFlagsIntern = modeADJ | Direct;

    QList<SenseSystem::cChannelSettings*> channelSettings = senseSettings->getChannelSettings();
    QList<SenseChannelCommon*> channels;
    channels.append(new MtXXXs2SenseChannel(scpi, "V", channelSettings.at(0), 0, ctrlFactory));
    channels.append(new MtXXXs2SenseChannel(scpi, "V", channelSettings.at(1), 1, ctrlFactory));
    channels.append(new MtXXXs2SenseChannel(scpi, "V", channelSettings.at(2), 2, ctrlFactory));
    channels.append(new MtXXXs2SenseChannel(scpi, "V", channelSettings.at(6), 6, ctrlFactory));
    channels.append(new MtXXXs2SenseChannel(scpi, "A", channelSettings.at(3), 3, ctrlFactory));
    channels.append(new MtXXXs2SenseChannel(scpi, "A", channelSettings.at(4), 4, ctrlFactory));
    channels.append(new MtXXXs2SenseChannel(scpi, "A", channelSettings.at(5), 5, ctrlFactory));
    channels.append(new MtXXXs2SenseChannel(scpi, "A", channelSettings.at(7), 7, ctrlFactory));

    for (int i = 0; i < 4; i++) {
        const QString &channelName = channels.at(i)->getName();
        QList<SenseRangeCommon*> rngListUAll;

        tryAddRange(rngListUAll,  "250V", channelName, adjData, scpi, true, 250.0, 4415057, 1.25, 0, rangeFlagsDevice);
        tryAddRange(rngListUAll,    "8V", channelName, adjData, scpi, true,   8.0, 3355443, 1.25, 1, rangeFlagsDevice);
        tryAddRange(rngListUAll, "100mV", channelName, adjData, scpi, true,   0.1, 4026532, 1.25, 2, rangeFlagsDevice);

        channels.at(i)->setRangeList(rngListUAll);
    }

    for (int i = 4; i < 7; i++) {
        const QString &channelName = channels.at(i)->getName();
        QList<SenseRangeCommon*> rngListIExceptAux;

        tryAddRange(rngListIExceptAux,   "10A", channelName, adjData, scpi, true,  10.0, 3197613, 1.25,  0, rangeFlagsDevice);
        tryAddRange(rngListIExceptAux,    "5A", channelName, adjData, scpi, true,   5.0, 3197613, 1.25,  1, rangeFlagsDevice);
        tryAddRange(rngListIExceptAux,  "2.5A", channelName, adjData, scpi, true,   2.5, 3997016, 1.25,  2, rangeFlagsDevice);
        tryAddRange(rngListIExceptAux,  "1.0A", channelName, adjData, scpi, true,   1.0, 4177527, 1.25,  3, rangeFlagsDevice);
        tryAddRange(rngListIExceptAux, "500mA", channelName, adjData, scpi, true,   0.5, 4177527, 1.25,  4, rangeFlagsDevice);
        tryAddRange(rngListIExceptAux, "250mA", channelName, adjData, scpi, true,  0.25, 4177527, 1.25,  5, rangeFlagsDevice);
        tryAddRange(rngListIExceptAux, "100mA", channelName, adjData, scpi, true,   0.1, 4177527, 1.25,  6, rangeFlagsDevice);
        tryAddRange(rngListIExceptAux,  "50mA", channelName, adjData, scpi, true,  0.05, 4177527, 1.25,  7, rangeFlagsDevice);
        tryAddRange(rngListIExceptAux,  "25mA", channelName, adjData, scpi, true, 0.025, 4177527, 1.25,  8, rangeFlagsDevice);

        tryAddRange(rngListIExceptAux,    "8V", channelName, adjData, scpi, true,   8.0, 3355443, 1.25,  9, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux,    "5V", channelName, adjData, scpi, true,   5.0, 4194304, 1.25, 10, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux,    "2V", channelName, adjData, scpi, true,   2.0, 2835586, 1.25, 11, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux,    "1V", channelName, adjData, scpi, true,   1.0, 2835586, 1.25, 12, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux, "500mV", channelName, adjData, scpi, true,   0.5, 3544482, 1.25, 13, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux, "200mV", channelName, adjData, scpi, true,   0.2, 2684355, 1.25, 14, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux, "100mV", channelName, adjData, scpi, true,   0.1, 3355443, 1.25, 15, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux,  "50mV", channelName, adjData, scpi, true,  0.05, 3355443, 1.25, 16, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux,  "20mV", channelName, adjData, scpi, true,  0.02, 2684355, 1.25, 17, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux,  "10mV", channelName, adjData, scpi, true,  0.01, 3355443, 1.25, 18, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux,   "5mV", channelName, adjData, scpi, true, 0.005, 3355443, 1.25, 19, rangeFlagsIntern);
        tryAddRange(rngListIExceptAux,   "2mV", channelName, adjData, scpi, true, 0.002, 2684355, 1.25, 20, rangeFlagsIntern);

        channels.at(i)->setRangeList(rngListIExceptAux);
    }

    // IAUX
    constexpr int channelNum = 7;
    const QString channelName = channels.at(channelNum)->getName();
    QList<SenseRangeCommon*> rngListIAux;
    // rValue = 1e-12 (yes a pico Ampere error - for 100% range sample value):
    // We did not yet find the place but it seems 0.0 turns into a divisor causing DSP to run almost infinite
    // loop on range change when running power1module on a channnel supporting 0A
    tryAddRange(rngListIAux, SenseRangeCommon::rangeNameCurrentNull, channelName, adjData, scpi, true, 1e-12, 3197613, 1.25, 0, rangeFlagsDevice);
    tryAddRange(rngListIAux,    "8V", channelName, adjData, scpi, true,   8.0, 3355443, 1.25,  9, rangeFlagsIntern);
    tryAddRange(rngListIAux,    "5V", channelName, adjData, scpi, true,   5.0, 4194304, 1.25, 10, rangeFlagsIntern);
    tryAddRange(rngListIAux,    "2V", channelName, adjData, scpi, true,   2.0, 2835586, 1.25, 11, rangeFlagsIntern);
    tryAddRange(rngListIAux,    "1V", channelName, adjData, scpi, true,   1.0, 2835586, 1.25, 12, rangeFlagsIntern);
    tryAddRange(rngListIAux, "500mV", channelName, adjData, scpi, true,   0.5, 3544482, 1.25, 13, rangeFlagsIntern);
    tryAddRange(rngListIAux, "200mV", channelName, adjData, scpi, true,   0.2, 2684355, 1.25, 14, rangeFlagsIntern);
    tryAddRange(rngListIAux, "100mV", channelName, adjData, scpi, true,   0.1, 3355443, 1.25, 15, rangeFlagsIntern);
    tryAddRange(rngListIAux,  "50mV", channelName, adjData, scpi, true,  0.05, 3355443, 1.25, 16, rangeFlagsIntern);
    tryAddRange(rngListIAux,  "20mV", channelName, adjData, scpi, true,  0.02, 2684355, 1.25, 17, rangeFlagsIntern);
    tryAddRange(rngListIAux,  "10mV", channelName, adjData, scpi, true,  0.01, 3355443, 1.25, 18, rangeFlagsIntern);
    tryAddRange(rngListIAux,   "5mV", channelName, adjData, scpi, true, 0.005, 3355443, 1.25, 19, rangeFlagsIntern);
    tryAddRange(rngListIAux,   "2mV", channelName, adjData, scpi, true, 0.002, 2684355, 1.25, 20, rangeFlagsIntern);

    channels.at(channelNum)->setRangeList(rngListIAux);
    return channels;
}

void MT310s2ChannelRangeFactory::tryAddRange(QList<SenseRangeCommon *> &rngList,
                                             const QString &rangeName,
                                             const QString &channelName,
                                             AdjDataPtr adjData,
                                             std::shared_ptr<cSCPI> scpi,
                                             bool avail,
                                             double nominalValue,
                                             int nominalSampleValue,
                                             double overloadFactor,
                                             quint8 controllerSelectionNum,
                                             quint16 supportedMeasModeMask)
{
    if(isInvalidAdjDataOrChannelRangeAvail(adjData, channelName, rangeName)) {
        rngList.append(new SenseRangeCommon(scpi,
                                            rangeName,
                                            avail,
                                            nominalValue,
                                            nominalSampleValue,
                                            round(nominalSampleValue * overloadFactor),
                                            controllerSelectionNum,
                                            supportedMeasModeMask,
                                            new AdjRangeScpi(scpi, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter()),
                                            rejectionScpiQueryDigitsMtXXXs2));
    }
}

bool MT310s2ChannelRangeFactory::isInvalidAdjDataOrChannelRangeAvail(AdjDataPtr adjData, QString channelName, QString rangeName)
{
    if(adjData->isEmpty())
        return true;
    return adjData->isChannelRangeAvailable(channelName, rangeName);
}
