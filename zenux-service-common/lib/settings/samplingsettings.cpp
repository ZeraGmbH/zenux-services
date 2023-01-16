#include "samplingsettings.h"
#include <xmlconfigreader.h>

enum configstate
{
    cfgAlias,
    cfgAvail
};

cSamplingSettings::cSamplingSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    SamplingSystem::cChannelSettings* settings = new SamplingSystem::cChannelSettings;
    m_ChannelSettingsList.append(settings);

    m_ConfigXMLMap["serviceconfig:resource:sample:s0:alias"] = cfgAlias;
    m_ConfigXMLMap["serviceconfig:resource:sample:s0:avail"] = cfgAvail;
}

cSamplingSettings::~cSamplingSettings()
{
    for(auto channel : qAsConst(m_ChannelSettingsList))
        delete channel;
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
        case cfgAlias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfgAvail:
            m_ChannelSettingsList.at(0)->m_bAvail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
