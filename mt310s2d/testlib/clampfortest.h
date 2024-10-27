#ifndef CLAMPFORTEST_H
#define CLAMPFORTEST_H

#include "clamp.h"

class ClampForTest : public cClamp
{
public:
    ClampForTest(PCBServer *server,
                 I2cSettings *i2cSettings,
                 SenseInterfaceCommon *senseInterface,
                 QString channelName,
                 quint8 ctrlChannel,
                 I2cMuxerInterface::Ptr i2cMuxer,
                 quint8 ctrlChannelSecondary);
};

#endif // CLAMPFORTEST_H
