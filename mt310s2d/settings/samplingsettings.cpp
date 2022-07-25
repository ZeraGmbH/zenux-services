#include "samplingsettings.h"
#include <xmlconfigreader.h>

cSamplingSettings::cSamplingSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    SamplingSystem::cChannelSettings* settings = new SamplingSystem::cChannelSettings;
    m_ChannelSettingsList.append(settings);

    m_ConfigXMLMap["mt310s2dconfig:resource:sample:s0:alias"] = SamplingSystem::cfgAlias;
    m_ConfigXMLMap["mt310s2dconfig:resource:sample:s0:avail"] = SamplingSystem::cfgAvail;
}

cSamplingSettings::~cSamplingSettings()
{
    for(auto channel : m_ChannelSettingsList) {
        delete channel;
    }
}

QList<SamplingSystem::cChannelSettings *> &cSamplingSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}

void cSamplingSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case SamplingSystem::cfgAlias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case SamplingSystem::cfgAvail:
            m_ChannelSettingsList.at(0)->m_bAvail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
