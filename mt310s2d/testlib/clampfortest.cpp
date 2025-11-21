#include "clampfortest.h"
#include "clampfactorytest.h"

ClampForTest::ClampForTest(PCBServer *server,
                           I2cSettings *i2cSettings,
                           SenseInterfaceCommon *senseInterface,
                           const SenseSystem::cChannelSettings *chSettings,
                           I2cMuxerInterface::Ptr i2cMuxer,
                           quint8 ctrlChannelSecondary) :
    cClamp(server,
           i2cSettings,
           senseInterface,
           chSettings,
           i2cMuxer,
           ctrlChannelSecondary,
           ClampFactoryTest::getTestClampType())
{
}
