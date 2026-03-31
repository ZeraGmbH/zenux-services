#include "mt581generatorchannelfactory.h"

QList<GeneratorChannelInterface *> Mt581GeneratorChannelFactory::createChannelsAndRanges(cSenseSettingsPtr senseSettings,
                                                                                std::shared_ptr<cSCPI> scpi,
                                                                                AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    const QList<SenseSystem::cChannelSettings*> &channelsSettings = senseSettings->getChannelSettings();
    QList<GeneratorChannelInterface *> generatorChannels;
    for (const SenseSystem::cChannelSettings* channelSettings : channelsSettings) {
        // Hack: this should come from settings
        if (channelSettings->m_nameMx == "m6" || channelSettings->m_nameMx == "m7")
            continue;

        generatorChannels.append(new GeneratorChannelInterface(scpi, senseSettings, channelSettings, ctrlFactory));
        // ranges???
    }
    return generatorChannels;
}
