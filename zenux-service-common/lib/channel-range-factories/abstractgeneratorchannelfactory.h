#ifndef ABSTRACTGENERATORCHANNELFACTORY_H
#define ABSTRACTGENERATORCHANNELFACTORY_H

#include "abstractfactoryi2cctrl.h"
#include "generatorchannelinterface.h"
#include "sensesettings.h"
#include <scpi.h>
#include <QList>

class AbstractGeneratorChannelFactory
{
public:
    virtual ~AbstractGeneratorChannelFactory() = default;
    virtual QList<GeneratorChannelInterface*> createChannelsAndRanges(const cSenseSettingsPtr &senseSettings,
                                                                      const std::shared_ptr<cSCPI> &scpi,
                                                                      const AbstractFactoryI2cCtrlPtr &ctrlFactory) = 0;
};

#endif // ABSTRACTGENERATORCHANNELFACTORY_H
