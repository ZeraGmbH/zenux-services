#include "mt581generatorchannelfactory.h"

QList<GeneratorChannel *> Mt581GeneratorChannelFactory::createChannelsAndRanges(cSenseSettingsPtr senseSettings,
                                                                                std::shared_ptr<cSCPI> scpi,
                                                                                AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    const QList<SenseSystem::cChannelSettings*> &channelsSettings = senseSettings->getChannelSettings();
    QList<GeneratorChannel *> generatorChannels;
    for (const SenseSystem::cChannelSettings* channelSettings : channelsSettings) {
        // Hack: this should come from settings
        if (channelSettings->m_nameMx == "m6" || channelSettings->m_nameMx == "m7")
            continue;

        generatorChannels.append(new GeneratorChannel(scpi, senseSettings, channelSettings, ctrlFactory));
        // ranges???
    }
    return generatorChannels;
}
