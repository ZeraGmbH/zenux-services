#include "clampfactory.h"

std::function<cClamp*(PCBServer *server,
                       I2cSettings *i2cSettings,
                       SenseInterfaceCommon *senseInterface,
                       const SenseSystem::cChannelSettings *chSettings,
                       I2cMuxerInterface::Ptr i2cMuxer,
                       quint8 ctrlChannelSecondary)>
    ClampFactory::m_createFunction = [](PCBServer *server,
                                        I2cSettings *i2cSettings,
                                        SenseInterfaceCommon *senseInterface,
                                        const SenseSystem::cChannelSettings *chSettings,
                                        I2cMuxerInterface::Ptr i2cMuxer,
                                        quint8 ctrlChannelSecondary) {
        return new cClamp(server, i2cSettings, senseInterface, chSettings, i2cMuxer, ctrlChannelSecondary);
    };

cClamp *ClampFactory::createClamp(PCBServer *server,
                                  I2cSettings *i2cSettings,
                                  SenseInterfaceCommon *senseInterface,
                                  const SenseSystem::cChannelSettings *chSettings,
                                  I2cMuxerInterface::Ptr i2cMuxer,
                                  quint8 ctrlChannelSecondary)
{
    return m_createFunction(server,
                            i2cSettings,
                            senseInterface,
                            chSettings,
                            i2cMuxer,
                            ctrlChannelSecondary);
}
