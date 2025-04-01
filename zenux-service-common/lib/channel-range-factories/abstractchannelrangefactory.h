#ifndef ABSTRACTCHANNELRANGEFACTORY_H
#define ABSTRACTCHANNELRANGEFACTORY_H

#include "sensechannelcommon.h"
#include "adjdatacompleteinternstream.h"
#include <QList>

class AbstractChannelRangeFactory
{
public:
    virtual ~AbstractChannelRangeFactory() = default;
    virtual QList<SenseChannelCommon*> createChannelAndRanges(cSenseSettings* senseSettings,
                                                              AdjDataPtr adjData,
                                                              std::shared_ptr<cSCPI> scpi,
                                                              AbstractFactoryI2cCtrlPtr ctrlFactory) = 0;
};

typedef std::shared_ptr<AbstractChannelRangeFactory> AbstractChannelRangeFactoryPtr;

#endif // ABSTRACTCHANNELRANGEFACTORY_H
