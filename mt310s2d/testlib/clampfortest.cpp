#include "clampfortest.h"
#include "clampfactorytest.h"

ClampForTest::ClampForTest(PCBServer *server,
                           I2cSettings *i2cSettings,
                           SenseInterfaceCommon *senseInterface,
                           QString channelName,
                           quint8 ctrlChannel,
                           I2cMuxerInterface::Ptr i2cMuxer,
                           quint8 ctrlChannelSecondary) :
    cClamp(server,
           i2cSettings,
           senseInterface,
           channelName,
           ctrlChannel,
           i2cMuxer,
           ctrlChannelSecondary,
           ClampFactoryTest::getTestClampType())
{
}
