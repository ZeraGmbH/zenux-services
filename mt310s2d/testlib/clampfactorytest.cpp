#include "clampfactorytest.h"
#include "clampfortest.h"

int ClampFactoryTest::m_clampForTestType = CL800ADC1000VDC;

void ClampFactoryTest::enableTest()
{
    m_createFunction = [](cPCBServer *server,
                          I2cSettings *i2cSettings,
                          SenseInterfaceCommon *senseInterface,
                          QString channelName,
                          quint8 ctrlChannel,
                          I2cMuxerInterface::Ptr i2cMuxer,
                          quint8 ctrlChannelSecondary) {
        return new ClampForTest(server, i2cSettings, senseInterface, channelName, ctrlChannel, i2cMuxer, ctrlChannelSecondary);
    };
}

void ClampFactoryTest::setTestClampType(int clampTypeNo)
{
    m_clampForTestType = clampTypeNo;
}

int ClampFactoryTest::getTestClampType()
{
    return m_clampForTestType;
}
