#include "samplingsettings.h"

enum configstate
{
    cfgAvail,
    cfgPllChannels
};

SamplingSettings::SamplingSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    ChannelSettings* settings = new ChannelSettings;
    m_ChannelSettingsList.append(settings);

    m_ConfigXMLMap["serviceconfig:resource:sample:s0:avail"] = cfgAvail;
    m_ConfigXMLMap["serviceconfig:resource:sample:s0:pllchannels"] = cfgPllChannels;
}

SamplingSettings::~SamplingSettings()
{
    for(auto channel : qAsConst(m_ChannelSettingsList))
        delete channel;
}

const QList<SamplingSettings::ChannelSettings *> &SamplingSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}

void SamplingSettings::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case cfgAvail:
            m_ChannelSettingsList.at(0)->m_bAvail = (m_pXMLReader->getValue(key) == "true");
            break;
        case cfgPllChannels:
            m_ChannelSettingsList.at(0)->m_pllChannels = QStringList(m_pXMLReader->getValue(key).split(','));
        }
    }
}
