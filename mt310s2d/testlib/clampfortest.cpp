#include "clampfortest.h"
#include "clampfactorytest.h"

ClampForTest::ClampForTest(cPCBServer *server,
                           cI2CSettings *i2cSettings,
                           Mt310s2SenseInterface *senseInterface,
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
