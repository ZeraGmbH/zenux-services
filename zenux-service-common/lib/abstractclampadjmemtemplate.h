#ifndef ABSTRACTCLAMPADJMEMTEMPLATE_H
#define ABSTRACTCLAMPADJMEMTEMPLATE_H

#include "clamp.h"

class AbstractClampAdjMemTemplate
{
public:
    virtual ~AbstractClampAdjMemTemplate() = default;
    bool isMemAvailable() const;
    cClamp::ClampTypes getClampType() const;

    cClamp* createClamp(PCBServer *server,
                        I2cSettings *i2cSettings,
                        SenseInterfaceCommon *senseInterface,
                        const SenseSystem::cChannelSettings *chSettings,
                        I2cMuxerInterface::Ptr i2cMuxer,
                        quint8 ctrlChannelSecondary);
};

typedef std::shared_ptr<AbstractClampAdjMemTemplate> AbstractClampFactoryPtr;

#endif // ABSTRACTCLAMPADJMEMTEMPLATE_H
