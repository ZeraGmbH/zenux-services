#include "sensesettings.h"
#include <xmlsettings.h>

cSenseSettings::cSenseSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    SenseSystem::cChannelSettings *settings;
    for (int i = 0; i < 6; i++)
    {
        m_ChannelSettingsList.append(settings = new SenseSystem::cChannelSettings);
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:alias1").arg(i)] = SenseSystem::cfg0Alias1 + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:alias2").arg(i)] = SenseSystem::cfg0Alias2 + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:ctrlchannel").arg(i)] = SenseSystem::cfg0ctrlchannel + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:dspchannel").arg(i)] = SenseSystem::cfg0dspchannel + i;
        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:overloadbit").arg(i)] = SenseSystem::cfg0overloadbit + i;

        m_ConfigXMLMap[QString("com5003dconfig:resource:sense:m%1:avail").arg(i)] = SenseSystem::cfg0avail + i;
    }
}


cSenseSettings::~cSenseSettings()
{
    for (int i = 0; i < m_ChannelSettingsList.count(); i++)
        delete m_ChannelSettingsList.at(i);
}


QList<SenseSystem::cChannelSettings*> &cSenseSettings::getChannelSettings()
{
    return m_ChannelSettingsList;
}


void cSenseSettings::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case SenseSystem::cfg0Alias1:
            m_ChannelSettingsList.at(0)->m_sAlias[0] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg1Alias1:
            m_ChannelSettingsList.at(1)->m_sAlias[0] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg2Alias1:
            m_ChannelSettingsList.at(2)->m_sAlias[0] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg3Alias1:
            m_ChannelSettingsList.at(3)->m_sAlias[0] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg4Alias1:
            m_ChannelSettingsList.at(4)->m_sAlias[0] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg5Alias1:
            m_ChannelSettingsList.at(5)->m_sAlias[0] = m_pXMLReader->getValue(key);
            break;

        case SenseSystem::cfg0Alias2:
            m_ChannelSettingsList.at(0)->m_sAlias[1] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg1Alias2:
            m_ChannelSettingsList.at(1)->m_sAlias[1] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg2Alias2:
            m_ChannelSettingsList.at(2)->m_sAlias[1] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg3Alias2:
            m_ChannelSettingsList.at(3)->m_sAlias[1] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg4Alias2:
            m_ChannelSettingsList.at(4)->m_sAlias[1] = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg5Alias2:
            m_ChannelSettingsList.at(5)->m_sAlias[1] = m_pXMLReader->getValue(key);
            break;

        case SenseSystem::cfg0ctrlchannel:
            m_ChannelSettingsList.at(0)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg1ctrlchannel:
            m_ChannelSettingsList.at(1)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg2ctrlchannel:
            m_ChannelSettingsList.at(2)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg3ctrlchannel:
            m_ChannelSettingsList.at(3)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg4ctrlchannel:
            m_ChannelSettingsList.at(4)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg5ctrlchannel:
            m_ChannelSettingsList.at(5)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt(&ok);
            break;

        case SenseSystem::cfg0dspchannel:
            m_ChannelSettingsList.at(0)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg1dspchannel:
            m_ChannelSettingsList.at(1)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg2dspchannel:
            m_ChannelSettingsList.at(2)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg3dspchannel:
            m_ChannelSettingsList.at(3)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg4dspchannel:
            m_ChannelSettingsList.at(4)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg5dspchannel:
            m_ChannelSettingsList.at(5)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;

        case SenseSystem::cfg0overloadbit:
            m_ChannelSettingsList.at(0)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg1overloadbit:
            m_ChannelSettingsList.at(1)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg2overloadbit:
            m_ChannelSettingsList.at(2)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg3overloadbit:
            m_ChannelSettingsList.at(3)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg4overloadbit:
            m_ChannelSettingsList.at(4)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case SenseSystem::cfg5overloadbit:
            m_ChannelSettingsList.at(5)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt(&ok);
            break;

        case SenseSystem::cfg0avail:
            m_ChannelSettingsList.at(0)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case SenseSystem::cfg1avail:
            m_ChannelSettingsList.at(1)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case SenseSystem::cfg2avail:
            m_ChannelSettingsList.at(2)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case SenseSystem::cfg3avail:
            m_ChannelSettingsList.at(3)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case SenseSystem::cfg4avail:
            m_ChannelSettingsList.at(4)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case SenseSystem::cfg5avail:
            m_ChannelSettingsList.at(5)->avail = (m_pXMLReader->getValue(key) == "true");
            break;

        }
    }
}
