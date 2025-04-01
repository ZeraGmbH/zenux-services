#ifndef COM5003CHANNELRANGEFACTORY_H
#define COM5003CHANNELRANGEFACTORY_H

#include "abstractchannelrangefactory.h"

class COM5003ChannelRangeFactory : public AbstractChannelRangeFactory
{
public:
    virtual QList<SenseChannelCommon*> createChannelAndRanges(cSenseSettings* senseSettings,
                                                               AdjDataPtr adjData,
                                                               std::shared_ptr<cSCPI> scpi,
                                                               AbstractFactoryI2cCtrlPtr ctrlFactory) override;
};

#endif // COM5003CHANNELRANGEFACTORY_H
