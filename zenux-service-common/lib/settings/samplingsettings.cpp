#include "samplingsettings.h"

enum configstate
{
    cfgAlias,
    cfgAvail
};

SamplingSettings::SamplingSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    ChannelSettings* settings = new ChannelSettings;
    m_ChannelSettingsList.append(settings);

    m_ConfigXMLMap["serviceconfig:resource:sample:s0:alias"] = cfgAlias;
    m_ConfigXMLMap["serviceconfig:resource:sample:s0:avail"] = cfgAvail;
}

SamplingSettings::~SamplingSettings()
{
    for(auto channel : qAsConst(m_ChannelSettingsList))
        delete channel;
}

QList<SamplingSettings::ChannelSettings *> &SamplingSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}

void SamplingSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case cfgAlias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfgAvail:
            m_ChannelSettingsList.at(0)->m_bAvail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
