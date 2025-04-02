#include "com5003channelrangefactory.h"
#include "senseinterfacecommon.h"
#include "com5003sensechannel.h"
#include <cmath>

QList<SenseChannelCommon *> COM5003ChannelRangeFactory::createChannelAndRanges(cSenseSettings *senseSettings,
                                                                               AdjDataPtr adjData,
                                                                               std::shared_ptr<cSCPI> scpi,
                                                                               AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    qInfo("Set channels/ranges for COM5003");
    constexpr quint16 rangeFlagsDevice = modeAC;
    constexpr quint16 rangeFlagsReference = modeREF;
    Q_UNUSED(adjData) // not use in com5003 for same signature as mt310s2
    //constexpr int rangeFlagsIntern = 0; // not used yet no clamp plugs

    QList<SenseSystem::cChannelSettings*> channelSettings = senseSettings->getChannelSettings();
    QList<SenseChannelCommon*> channels;
    channels.append(new Com5003SenseChannel(scpi, "V", channelSettings.at(0), 0, ctrlFactory));
    channels.append(new Com5003SenseChannel(scpi, "V", channelSettings.at(1), 1, ctrlFactory));
    channels.append(new Com5003SenseChannel(scpi, "V", channelSettings.at(2), 2, ctrlFactory));
    channels.append(new Com5003SenseChannel(scpi, "A", channelSettings.at(3), 3, ctrlFactory));
    channels.append(new Com5003SenseChannel(scpi, "A", channelSettings.at(4), 4, ctrlFactory));
    channels.append(new Com5003SenseChannel(scpi, "A", channelSettings.at(5), 5, ctrlFactory));

    for (int i = 0; i < 3; i++) {
        QList<SenseRangeCommon*> rngListU;
        tryAddRange(rngListU, scpi,  "480V", true, 480.0, 4712563,   1.25, 0, rangeFlagsDevice);
        tryAddRange(rngListU, scpi,  "240V", true, 240.0, 4712563,   1.25, 1, rangeFlagsDevice);
        tryAddRange(rngListU, scpi,  "120V", true, 120.0, 4712563,   1.25, 2, rangeFlagsDevice);
        tryAddRange(rngListU, scpi,   "60V", true,  60.0, 4712563,   1.25, 3, rangeFlagsDevice);

        tryAddRange(rngListU, scpi,   "12V", true,  12.0, 3887864.8, 1.25, 4, rangeFlagsDevice);
        tryAddRange(rngListU, scpi,    "5V", true,   5.0, 4516206,   1.25, 5, rangeFlagsDevice);

        tryAddRange(rngListU, scpi,   "R0V", false,  9.0, 3839668,  1.389, 14, rangeFlagsReference);
        tryAddRange(rngListU, scpi,  "R10V", false, 10.0, 4266298,   1.25, 15, rangeFlagsReference);
        channels.at(i)->setRangeList(rngListU);
    }

    for (int i = 3; i < 6; i++) {
        QList<SenseRangeCommon*> rngListI;
        tryAddRange(rngListI, scpi,  "200A", true, 200.0, 6257236, 0.84, 0, rangeFlagsDevice); // 168 A max ???
        tryAddRange(rngListI, scpi,  "100A", true, 100.0, 4692928, 1.25, 1, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,   "50A", true,  50.0, 4692928, 1.25, 2, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,   "25A", true, 25.0 , 4692928, 1.25, 3, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,   "10A", true, 10.0 , 4692928, 1.25, 4, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,    "5A", true,  5.0 , 4692928, 1.25, 5, rangeFlagsDevice);

        tryAddRange(rngListI, scpi,  "2.5A", true, 2.5  , 4692928, 1.25, 6, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,  "1.0A", true, 1.0  , 4692928, 1.25, 7, rangeFlagsDevice);
        tryAddRange(rngListI, scpi, "500mA", true, 0.5  , 4692928, 1.25, 8, rangeFlagsDevice);
        tryAddRange(rngListI, scpi, "250mA", true, 0.25 , 4692928, 1.25, 9, rangeFlagsDevice);
        tryAddRange(rngListI, scpi, "100mA", true, 0.1  , 4692928, 1.25, 10, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,  "50mA", true, 0.05 , 4692928, 1.25, 11, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,  "25mA", true, 0.025, 4692928, 1.25, 12, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,  "10mA", true, 0.01 , 4692928, 1.25, 13, rangeFlagsDevice);
        tryAddRange(rngListI, scpi,   "5mA", true, 0.005, 4692928, 1.25, 14, rangeFlagsDevice);

        tryAddRange(rngListI, scpi,   "R0V", false,  9.0, 3839668, 1.389,15, rangeFlagsReference); // overload: don't care
        tryAddRange(rngListI, scpi,  "R10V", false, 10.0, 4266298, 1.25, 16, rangeFlagsReference);

        channels.at(i)->setRangeList(rngListI);
    }
    return channels;
}

void COM5003ChannelRangeFactory::tryAddRange(QList<SenseRangeCommon *> &rngList,
                                             std::shared_ptr<cSCPI> scpi,
                                             const QString &rangeName,
                                             bool avail,
                                             double nominalValue,
                                             double nominalSampleValue, // we have float values around for rejection ...???
                                             double overloadFactor,
                                             quint8 controllerSelectionNum,
                                             quint16 supportedMeasModeMask)
{
    rngList.append(new SenseRangeCommon(scpi,
                                        rangeName,
                                        avail,
                                        nominalValue,
                                        nominalSampleValue,
                                        round(nominalSampleValue * overloadFactor),
                                        controllerSelectionNum,
                                        supportedMeasModeMask,
                                        new AdjRangeScpi(scpi, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter()),
                                        rejectionScpiQueryDigitsCom5003));
}
