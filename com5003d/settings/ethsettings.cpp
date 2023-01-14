#include "ethsettings.h"
#include <xmlconfigreader.h>

EthSettingsPcb::EthSettingsPcb(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["pcbdconfig:connectivity:ethernet:ipadress:resourcemanager"] = setRMIPAdress;
    m_ConfigXMLMap["pcbdconfig:connectivity:ethernet:port:protobufserver"] = setProtobufServerPort;
    m_ConfigXMLMap["pcbdconfig:connectivity:ethernet:port:scpiserver"] = setSCPIServerPort;
    m_ConfigXMLMap["pcbdconfig:connectivity:ethernet:port:resourcemanager"] = setRMPort;
    m_ConfigXMLMap["pcbdconfig:connectivity:ethernet:scpiactive"] = setSCPIactive;
}

QString EthSettingsPcb::getRMIPadr()
{
    return m_sRMIPAdr;
}

quint16 EthSettingsPcb::getPort(ethmember member)
{
    quint16 port;
    switch (member)
    {
    case protobufserver:
        port = m_nProtobufServerPort;
        break;
    case scpiserver:
        port = m_nSCPIServerPort;
        break;
    case resourcemanager:
        port = m_nRMPort;
        break;
    }
    return port;
}

bool EthSettingsPcb::isSCPIactive()
{
    return m_bSCPIactive;
}

void EthSettingsPcb::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case setRMIPAdress:
            m_sRMIPAdr = m_pXMLReader->getValue(key);
            break;
        case setProtobufServerPort:
            m_nProtobufServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setSCPIServerPort:
            m_nSCPIServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setRMPort:
            m_nRMPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setSCPIactive:
            m_bSCPIactive = (m_pXMLReader->getValue(key).toInt() == 1);
            break;
        }
    }
}
