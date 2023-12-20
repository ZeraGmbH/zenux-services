#ifndef CLAMPFORTEST_H
#define CLAMPFORTEST_H

#include "clamp.h"

class ClampForTest : public cClamp
{
public:
    ClampForTest(cPCBServer *server,
                 cI2CSettings *i2cSettings,
                 Mt310s2SenseInterface *senseInterface,
                 QString channelName,
                 quint8 ctrlChannel,
                 I2cMuxerInterface::Ptr i2cMuxer,
                 quint8 ctrlChannelSecondary);
};

#endif // CLAMPFORTEST_H
