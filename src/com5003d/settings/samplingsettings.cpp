#include <xmlconfigreader.h>
#include <QVariant>

#include "samplingsettings.h"

cSamplingSettings::cSamplingSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    SamplingSystem::cChannelSettings* settings = new SamplingSystem::cChannelSettings;
    m_ChannelSettingsList.append(settings);

    m_ConfigXMLMap["com5003dconfig:resource:sample:s0:alias"] = SamplingSystem::cfgAlias;
    m_ConfigXMLMap["com5003dconfigresource:sample:s0:avail"] = SamplingSystem::cfgAvail;
}


cSamplingSettings::~cSamplingSettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<SamplingSystem::cChannelSettings *> &cSamplingSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}


void cSamplingSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key))
    {
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
