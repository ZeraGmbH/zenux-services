#include "hkeysettings.h"
#include <xmlconfigreader.h>
#include <QList>
#include <QVariant>

cHKeySettings::cHKeySettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    for (int i = 0; i < 1; i++)
    {
        m_ChannelSettingsList.append(new HKeySystem::cChannelSettings);
        m_ConfigXMLMap[QString("mt310s2dconfig:resource:hkey:hk%1:alias").arg(i)] = HKeySystem::cfgHK0Alias + i;
        m_ConfigXMLMap[QString("mt310s2dconfig:resource:hkey:hk%1:avail").arg(i)] = HKeySystem::cfgHK0avail + i;
    }
}


cHKeySettings::~cHKeySettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<HKeySystem::cChannelSettings*> &cHKeySettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}


void cHKeySettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case HKeySystem::cfgHK0Alias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case HKeySystem::cfgHK0avail:
            m_ChannelSettingsList.at(0)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
