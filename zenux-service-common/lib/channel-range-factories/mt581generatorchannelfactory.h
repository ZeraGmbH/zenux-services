#ifndef MT581GENERATORCHANNELFACTORY_H
#define MT581GENERATORCHANNELFACTORY_H

#include "abstractgeneratorchannelfactory.h"

class Mt581GeneratorChannelFactory : public AbstractGeneratorChannelFactory
{
public:
    QList<GeneratorChannelInterface*> createChannelsAndRanges(cSenseSettingsPtr senseSettings,
                                                     std::shared_ptr<cSCPI> scpi,
                                                     AbstractFactoryI2cCtrlPtr ctrlFactory) override;
};

#endif // MT581GENERATORCHANNELFACTORY_H
