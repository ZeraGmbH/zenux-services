#include "hkinsettings.h"

enum configstate
{
    cfgHK0Alias,
    cfgHK0avail,
};

HkInSettings::HkInSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    for (int i = 0; i < 1; i++) {
        m_ChannelSettingsList.append(new ChannelSettings);
        m_ConfigXMLMap[QString("serviceconfig:resource:hkey:hk%1:alias").arg(i)] = cfgHK0Alias + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:hkey:hk%1:avail").arg(i)] = cfgHK0avail + i;
    }
}

HkInSettings::~HkInSettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<HkInSettings::ChannelSettings*> &HkInSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}

void HkInSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case cfgHK0Alias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfgHK0avail:
            m_ChannelSettingsList.at(0)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
