#include "frqinputsettings.h"
#include <QList>
#include <QVariant>

cFRQInputSettings::cFRQInputSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    FRQInputSystem::cChannelSettings *settings;
    for (int i = 0; i < 4; i++)
    {
        m_ChannelSettingsList.append(settings = new FRQInputSystem::cChannelSettings);
        m_ConfigXMLMap[QString("com5003dconfig:resource:frqinput:fi%1:alias").arg(i)] = FRQInputSystem::cfgFin0Alias + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:frqinput:fi%1:avail").arg(i)] = FRQInputSystem::cfgFin0avail + i;
    }
}


cFRQInputSettings::~cFRQInputSettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<FRQInputSystem::cChannelSettings*> &cFRQInputSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}


void cFRQInputSettings::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case FRQInputSystem::cfgFin0Alias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case FRQInputSystem::cfgFin1Alias:
            m_ChannelSettingsList.at(1)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case FRQInputSystem::cfgFin2Alias:
            m_ChannelSettingsList.at(2)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case FRQInputSystem::cfgFin3Alias:
            m_ChannelSettingsList.at(3)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case FRQInputSystem::cfgFin0avail:
            m_ChannelSettingsList.at(0)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case FRQInputSystem::cfgFin1avail:
            m_ChannelSettingsList.at(1)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case FRQInputSystem::cfgFin2avail:
            m_ChannelSettingsList.at(2)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case FRQInputSystem::cfgFin3avail:
            m_ChannelSettingsList.at(3)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}


