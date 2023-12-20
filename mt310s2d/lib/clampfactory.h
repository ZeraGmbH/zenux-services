#ifndef CLAMPFACTORY_H
#define CLAMPFACTORY_H

#include "clamp.h"
#include <functional>

class ClampFactory
{
public:
    static cClamp* createClamp(cPCBServer *server,
                               cI2CSettings *i2cSettings,
                               Mt310s2SenseInterface *senseInterface,
                               QString channelName,
                               quint8 ctrlChannel,
                               I2cMuxerInterface::Ptr i2cMuxer,
                               quint8 ctrlChannelSecondary);
protected:
    static std::function<cClamp*(cPCBServer *server,
                                    cI2CSettings *i2cSettings,
                                    Mt310s2SenseInterface *senseInterface,
                                    QString channelName,
                                    quint8 ctrlChannel,
                                    I2cMuxerInterface::Ptr i2cMuxer,
                                  quint8 ctrlChannelSecondary)> m_createFunction;
};

#endif // CLAMPFACTORY_H
