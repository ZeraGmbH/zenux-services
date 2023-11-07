#include "foutsettings.h"

enum configstate
{
    cfg0Alias,
    cfg1Alias,
    cfg2Alias,
    cfg3Alias,
    cfg0dspserver,
    cfg1dspserver,
    cfg2dspserver,
    cfg3dspserver,
    cfg0dspchannel,
    cfg1dspchannel,
    cfg2dspchannel,
    cfg3dspchannel,
    cfg0avail,
    cfg1avail,
    cfg2avail,
    cfg3avail
};

FOutSettings::FOutSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    for (int i = 0; i < 4; i++) {
        m_ChannelSettingsList.append(new ChannelSettings);
        m_ConfigXMLMap[QString("serviceconfig:resource:source:fpzout:fo%1:alias").arg(i)] = cfg0Alias + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:source:fpzout:fo%1:dspserver").arg(i)] = cfg0dspserver + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:source:fpzout:fo%1:dspchannel").arg(i)] = cfg0dspchannel + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:source:fpzout:fo%1:avail").arg(i)] = cfg0avail + i;
    }
}

FOutSettings::~FOutSettings()
{
    for(auto channel : qAsConst(m_ChannelSettingsList))
        delete channel;
}

QList<FOutSettings::ChannelSettings*> &FOutSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}

void FOutSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case cfg0Alias:
            m_ChannelSettingsList.at(0)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfg1Alias:
            m_ChannelSettingsList.at(1)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfg2Alias:
            m_ChannelSettingsList.at(2)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfg3Alias:
            m_ChannelSettingsList.at(3)->m_sAlias = m_pXMLReader->getValue(key);
            break;
        case cfg0dspserver:
            m_ChannelSettingsList.at(0)->m_nDspServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case cfg1dspserver:
            m_ChannelSettingsList.at(1)->m_nDspServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case cfg2dspserver:
            m_ChannelSettingsList.at(2)->m_nDspServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case cfg3dspserver:
            m_ChannelSettingsList.at(3)->m_nDspServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case cfg0dspchannel:
            m_ChannelSettingsList.at(0)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case cfg1dspchannel:
            m_ChannelSettingsList.at(1)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case cfg2dspchannel:
            m_ChannelSettingsList.at(2)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case cfg3dspchannel:
            m_ChannelSettingsList.at(3)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case cfg0avail:
            m_ChannelSettingsList.at(0)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case cfg1avail:
            m_ChannelSettingsList.at(1)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case cfg2avail:
            m_ChannelSettingsList.at(2)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case cfg3avail:
            m_ChannelSettingsList.at(3)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
