#include "scinsettings.h"

enum configstate
{
    cfgSH0Alias,
    cfgSH0avail,
};

ScInSettings::ScInSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    for (int i = 0; i < 1; i++) {
        m_ChannelSettingsList.append(new ChannelSettings);
        m_ConfigXMLMap[QString("serviceconfig:resource:schead:sh%1:alias").arg(i)] = cfgSH0Alias + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:schead:sh%1:avail").arg(i)] = cfgSH0avail + i;
    }
}

ScInSettings::~ScInSettings()
{
    for(auto channel : qAsConst(m_ChannelSettingsList))
        delete channel;
}

QList<ScInSettings::ChannelSettings *> &ScInSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}

void ScInSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case cfgSH0Alias:
            m_ChannelSettingsList.at(0)->m_alias = m_pXMLReader->getValue(key);
            break;
        case cfgSH0avail:
            m_ChannelSettingsList.at(0)->m_avail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
