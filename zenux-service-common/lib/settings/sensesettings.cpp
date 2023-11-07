#include "sensesettings.h"
#include <xmlconfigreader.h>

cSenseSettings::cSenseSettings(Zera::XMLConfig::cReader *xmlread, int channelCount)
{
    m_pXMLReader = xmlread;
    for (int i = 0; i < channelCount; i++) {
        m_ChannelSettingsList.append(new SenseSystem::cChannelSettings);
        m_ConfigXMLMap[QString("serviceconfig:resource:sense:m%1:alias1").arg(i)] = SenseSystem::cfg0Alias1 + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:sense:m%1:alias2").arg(i)] = SenseSystem::cfg0Alias2 + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:sense:m%1:ctrlchannel").arg(i)] = SenseSystem::cfg0ctrlchannel + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:sense:m%1:dspchannel").arg(i)] = SenseSystem::cfg0dspchannel + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:sense:m%1:pluggedbit").arg(i)] = SenseSystem::cfg0pluggedbit + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:sense:m%1:muxchannel").arg(i)] = SenseSystem::cfg0muxchannelno + i;
        m_ConfigXMLMap[QString("serviceconfig:resource:sense:m%1:overloadbit").arg(i)] = SenseSystem::cfg0overloadbit + i;

        m_ConfigXMLMap[QString("serviceconfig:resource:sense:m%1:avail").arg(i)] = SenseSystem::cfg0avail + i;
    }
}

cSenseSettings::~cSenseSettings()
{
    for(auto channel : qAsConst(m_ChannelSettingsList))
        delete channel;
}

const QList<SenseSystem::cChannelSettings*> &cSenseSettings::getChannelSettings() const
{
    return m_ChannelSettingsList;
}

void cSenseSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case SenseSystem::cfg0Alias1:
            m_ChannelSettingsList.at(0)->m_sAlias1 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg1Alias1:
            m_ChannelSettingsList.at(1)->m_sAlias1 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg2Alias1:
            m_ChannelSettingsList.at(2)->m_sAlias1 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg3Alias1:
            m_ChannelSettingsList.at(3)->m_sAlias1 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg4Alias1:
            m_ChannelSettingsList.at(4)->m_sAlias1 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg5Alias1:
            m_ChannelSettingsList.at(5)->m_sAlias1 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg6Alias1:
            m_ChannelSettingsList.at(6)->m_sAlias1 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg7Alias1:
            m_ChannelSettingsList.at(7)->m_sAlias1 = m_pXMLReader->getValue(key);
            break;

        case SenseSystem::cfg0Alias2:
            m_ChannelSettingsList.at(0)->m_sAlias2 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg1Alias2:
            m_ChannelSettingsList.at(1)->m_sAlias2 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg2Alias2:
            m_ChannelSettingsList.at(2)->m_sAlias2 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg3Alias2:
            m_ChannelSettingsList.at(3)->m_sAlias2 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg4Alias2:
            m_ChannelSettingsList.at(4)->m_sAlias2 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg5Alias2:
            m_ChannelSettingsList.at(5)->m_sAlias2 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg6Alias2:
            m_ChannelSettingsList.at(6)->m_sAlias2 = m_pXMLReader->getValue(key);
            break;
        case SenseSystem::cfg7Alias2:
            m_ChannelSettingsList.at(7)->m_sAlias2 = m_pXMLReader->getValue(key);
            break;

        case SenseSystem::cfg0ctrlchannel:
            m_ChannelSettingsList.at(0)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg1ctrlchannel:
            m_ChannelSettingsList.at(1)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg2ctrlchannel:
            m_ChannelSettingsList.at(2)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg3ctrlchannel:
            m_ChannelSettingsList.at(3)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg4ctrlchannel:
            m_ChannelSettingsList.at(4)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg5ctrlchannel:
            m_ChannelSettingsList.at(5)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg6ctrlchannel:
            m_ChannelSettingsList.at(6)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg7ctrlchannel:
            m_ChannelSettingsList.at(7)->m_nCtrlChannel = m_pXMLReader->getValue(key).toInt();
            break;

        case SenseSystem::cfg0dspchannel:
            m_ChannelSettingsList.at(0)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg1dspchannel:
            m_ChannelSettingsList.at(1)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg2dspchannel:
            m_ChannelSettingsList.at(2)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg3dspchannel:
            m_ChannelSettingsList.at(3)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg4dspchannel:
            m_ChannelSettingsList.at(4)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg5dspchannel:
            m_ChannelSettingsList.at(5)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg6dspchannel:
            m_ChannelSettingsList.at(6)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg7dspchannel:
            m_ChannelSettingsList.at(7)->m_nDspChannel  =  m_pXMLReader->getValue(key).toInt();
            break;

        case SenseSystem::cfg0pluggedbit:
            m_ChannelSettingsList.at(0)->m_nPluggedBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg1pluggedbit:
            m_ChannelSettingsList.at(1)->m_nPluggedBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg2pluggedbit:
            m_ChannelSettingsList.at(2)->m_nPluggedBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg3pluggedbit:
            m_ChannelSettingsList.at(3)->m_nPluggedBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg4pluggedbit:
            m_ChannelSettingsList.at(4)->m_nPluggedBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg5pluggedbit:
            m_ChannelSettingsList.at(5)->m_nPluggedBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg6pluggedbit:
            m_ChannelSettingsList.at(6)->m_nPluggedBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg7pluggedbit:
            m_ChannelSettingsList.at(7)->m_nPluggedBit  =  m_pXMLReader->getValue(key).toInt();
            break;

        case SenseSystem::cfg0muxchannelno:
            m_ChannelSettingsList.at(0)->m_nMuxChannelNo =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg1muxchannelno:
            m_ChannelSettingsList.at(1)->m_nMuxChannelNo =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg2muxchannelno:
            m_ChannelSettingsList.at(2)->m_nMuxChannelNo =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg3muxchannelno:
            m_ChannelSettingsList.at(3)->m_nMuxChannelNo =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg4muxchannelno:
            m_ChannelSettingsList.at(4)->m_nMuxChannelNo =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg5muxchannelno:
            m_ChannelSettingsList.at(5)->m_nMuxChannelNo =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg6muxchannelno:
            m_ChannelSettingsList.at(6)->m_nMuxChannelNo =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg7muxchannelno:
            m_ChannelSettingsList.at(7)->m_nMuxChannelNo =  m_pXMLReader->getValue(key).toInt();
            break;

        case SenseSystem::cfg0overloadbit:
            m_ChannelSettingsList.at(0)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg1overloadbit:
            m_ChannelSettingsList.at(1)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg2overloadbit:
            m_ChannelSettingsList.at(2)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg3overloadbit:
            m_ChannelSettingsList.at(3)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg4overloadbit:
            m_ChannelSettingsList.at(4)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg5overloadbit:
            m_ChannelSettingsList.at(5)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg6overloadbit:
            m_ChannelSettingsList.at(6)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt();
            break;
        case SenseSystem::cfg7overloadbit:
            m_ChannelSettingsList.at(7)->m_nOverloadBit  =  m_pXMLReader->getValue(key).toInt();
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
        case SenseSystem::cfg6avail:
            m_ChannelSettingsList.at(6)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        case SenseSystem::cfg7avail:
            m_ChannelSettingsList.at(7)->avail = (m_pXMLReader->getValue(key) == "true");
            break;
        }
    }
}
