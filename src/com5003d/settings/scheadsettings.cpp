#include "scheadsettings.h"

cSCHeadSettings::cSCHeadSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    SCHeadSystem::cChannelSettings *settings;
    for (int i = 0; i < 1; i++)
    {
        m_ChannelSettingsList.append(settings = new SCHeadSystem::cChannelSettings);
        m_ConfigXMLMap[QString("com5003dconfig:resource:schead:sh%1:alias").arg(i)] = SCHeadSystem::cfgSH0Alias + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:schead:fi%1:avail").arg(i)] = SCHeadSystem::cfgSH0avail + i;
    }
}


cSCHeadSettings::~cSCHeadSettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<SCHeadSystem::cChannelSettings*> &cSCHeadSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}


void cSCHeadSettings::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case SCHeadSystem::cfgSH0Alias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case SCHeadSystem::cfgSH0avail:
            m_ChannelSettingsList.at(0)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
