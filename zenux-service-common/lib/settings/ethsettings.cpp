#include "ethsettings.h"

enum ethconfigstate
{
    setRmIpAdress,
    setProtobufServerPort,
    setScpiServerPort,
    setRmPort,
    setSCPIactive
};

EthSettings::EthSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["serviceconfig:connectivity:ethernet:ipadress:resourcemanager"] = setRmIpAdress;
    m_ConfigXMLMap["serviceconfig:connectivity:ethernet:port:protobufserver"] = setProtobufServerPort;
    m_ConfigXMLMap["serviceconfig:connectivity:ethernet:port:scpiserver"] = setScpiServerPort;
    m_ConfigXMLMap["serviceconfig:connectivity:ethernet:port:resourcemanager"] = setRmPort;
    m_ConfigXMLMap["serviceconfig:connectivity:ethernet:scpiactive"] = setSCPIactive;
}

QString EthSettings::getRMIPadr()
{
    return m_sRMIPAdr;
}

quint16 EthSettings::getPort(ethmember member)
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

bool EthSettings::isSCPIactive()
{
    return m_bSCPIactive;
}

void EthSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case setRmIpAdress:
            m_sRMIPAdr = m_pXMLReader->getValue(key);
            break;
        case setProtobufServerPort:
            m_nProtobufServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setScpiServerPort:
            m_nSCPIServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setRmPort:
            m_nRMPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setSCPIactive:
            m_bSCPIactive = (m_pXMLReader->getValue(key).toInt() == 1);
            break;
        }
    }
}
