#ifndef MT310S2CHANNELRANGEFACTORY_H
#define MT310S2CHANNELRANGEFACTORY_H

#include "abstractchannelrangefactory.h"

class MT310s2ChannelRangeFactory : public AbstractChannelRangeFactory
{
public:
    virtual QList<SenseChannelCommon*> createChannelAndRanges(cSenseSettingsPtr senseSettings,
                                                               AdjDataPtr adjData,
                                                               std::shared_ptr<cSCPI> scpi,
                                                               AbstractFactoryI2cCtrlPtr ctrlFactory) override;
private:
    static void tryAddRange(QList<SenseRangeCommon *> &rngList,
                            const QString &rangeName,
                            const QString &channelName,
                            AdjDataPtr adjData,
                            std::shared_ptr<cSCPI> scpi,
                            bool avail,
                            double nominalValue,
                            int nominalSampleValue,
                            double overloadFactor,
                            quint8 controllerSelectionNum,
                            quint16 supportedMeasModeMask);
    static bool isInvalidAdjDataOrChannelRangeAvail(AdjDataPtr adjData, QString channelName, QString rangeName);
};

#endif // MT310S2CHANNELRANGEFACTORY_H
