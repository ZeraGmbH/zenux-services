#ifndef MT581S2CHANNELRANGEFACTORY_H
#define MT581S2CHANNELRANGEFACTORY_H

#include "abstractchannelrangefactory.h"

class MT581s2ChannelRangeFactory : public AbstractChannelRangeFactory
{
public:
    virtual QList<SenseChannelCommon*> createChannelAndRanges(cSenseSettings* senseSettings,
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

#endif // MT581S2CHANNELRANGEFACTORY_H
