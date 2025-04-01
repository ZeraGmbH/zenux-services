#include "com5003channelrangefactory.h"
#include "senseinterfacecommon.h"
#include "com5003sensechannel.h"
#include "com5003senserange.h"

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
        rngListU.append(new Com5003SenseRange(scpi,  "480V", true, 480.0, 4712563.0, 5890704.0, 0, rangeFlagsDevice));
        rngListU.append(new Com5003SenseRange(scpi,  "240V", true, 240.0, 4712563.0, 5890704.0, 1, rangeFlagsDevice));
        rngListU.append(new Com5003SenseRange(scpi,  "120V", true, 120.0, 4712563.0, 5890704.0, 2, rangeFlagsDevice));
        rngListU.append(new Com5003SenseRange(scpi,   "60V", true,  60.0, 4712563.0, 5890704.0, 3, rangeFlagsDevice));

        rngListU.append(new Com5003SenseRange(scpi,   "12V", true,  12.0, 3887864.8, 4859831.0, 4, rangeFlagsDevice));
        rngListU.append(new Com5003SenseRange(scpi,    "5V", true,   5.0, 4516206.0, 5645258.0, 5, rangeFlagsDevice));

        rngListU.append(new Com5003SenseRange(scpi,   "R0V", false,  9.0, 3839668.2, 5332873.0, 14, rangeFlagsReference));
        rngListU.append(new Com5003SenseRange(scpi,  "R10V", false, 10.0, 4266298.0, 5332873.0, 15, rangeFlagsReference));
        channels.at(i)->setRangeList(rngListU);
    }

    for (int i = 3; i < 6; i++) {
        QList<SenseRangeCommon*> rngListI;
        rngListI.append(new Com5003SenseRange(scpi,  "200A", true, 200.0, 6257236.0, 5256077.0, 0, rangeFlagsDevice)); // 168 A max ???
        rngListI.append(new Com5003SenseRange(scpi,  "100A", true, 100.0, 4692928.0, 5866160.0, 1, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,   "50A", true,  50.0, 4692928.0, 5866160.0, 2, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,   "25A", true, 25.0 , 4692928.0, 5866160.0, 3, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,   "10A", true, 10.0 , 4692928.0, 5866160.0, 4, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,    "5A", true,  5.0 , 4692928.0, 5866160.0, 5, rangeFlagsDevice));

        rngListI.append(new Com5003SenseRange(scpi,  "2.5A", true, 2.5  , 4692928.0, 5866160.0, 6, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,  "1.0A", true, 1.0  , 4692928.0, 5866160.0, 7, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi, "500mA", true, 0.5  , 4692928.0, 5866160.0, 8, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi, "250mA", true, 0.25 , 4692928.0, 5866160.0, 9, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi, "100mA", true, 0.1  , 4692928.0, 5866160.0, 10, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,  "50mA", true, 0.05 , 4692928.0, 5866160.0, 11, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,  "25mA", true, 0.025, 4692928.0, 5866160.0, 12, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,  "10mA", true, 0.01 , 4692928.0, 5866160.0, 13, rangeFlagsDevice));
        rngListI.append(new Com5003SenseRange(scpi,   "5mA", true, 0.005, 4692928.0, 5866160.0, 14, rangeFlagsDevice));

        rngListI.append(new Com5003SenseRange(scpi,   "R0V", false,  9.0, 3839668.2, 5332873.0, 15, rangeFlagsReference));
        rngListI.append(new Com5003SenseRange(scpi,  "R10V", false, 10.0, 4266298.0, 5332873.0, 16, rangeFlagsReference));

        channels.at(i)->setRangeList(rngListI);
    }
    return channels;
}
