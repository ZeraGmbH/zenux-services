#include "finsettings.h"

enum configstate
{
    cfgFin0Alias,
    cfgFin1Alias,
    cfgFin2Alias,
    cfgFin3Alias,
    cfgFin0avail,
    cfgFin1avail,
    cfgFin2avail,
    cfgFin3avail,
};

FInSettings::FInSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    for (int i = 0; i < 4; i++) {
        m_ChannelSettingsList.append(new ChannelSettings);
        m_ConfigXMLMap[QString("serviceconfig:resource:frqinput:fi%1:alias").arg(i)] = cfgFin0Alias + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:frqinput:fi%1:avail").arg(i)] = cfgFin0avail + i;
    }
}

FInSettings::~FInSettings()
{
    for(auto channel : qAsConst(m_ChannelSettingsList)) {
        delete channel;
    }
}

QList<FInSettings::ChannelSettings *> &FInSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}

void FInSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case cfgFin0Alias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfgFin1Alias:
            m_ChannelSettingsList.at(1)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfgFin2Alias:
            m_ChannelSettingsList.at(2)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfgFin3Alias:
            m_ChannelSettingsList.at(3)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfgFin0avail:
            m_ChannelSettingsList.at(0)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case cfgFin1avail:
            m_ChannelSettingsList.at(1)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case cfgFin2avail:
            m_ChannelSettingsList.at(2)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case cfgFin3avail:
            m_ChannelSettingsList.at(3)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
