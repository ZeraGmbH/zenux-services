#include "ethsettingssec.h"

enum ethconfigstate
{
    setRmIpAdress,
    setProtobufServerPort,
    setRmPort
};

EthSettingsSec::EthSettingsSec(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["serviceconfig:connectivity:ethernet:ipadress:resourcemanager"] = setRmIpAdress;
    m_ConfigXMLMap["serviceconfig:connectivity:ethernet:port:server"] = setProtobufServerPort;
    m_ConfigXMLMap["serviceconfig:connectivity:ethernet:port:resourcemanager"] = setRmPort;
}

QString EthSettingsSec::getRMIPadr()
{
    return m_sRMIPAdr;
}

quint16 EthSettingsSec::getPort(ethmember member)
{
    quint16 port;
    switch (member)
    {
    case protobufserver:
        port = m_nServerPort;
        break;
    case resourcemanager:
        port = m_nRMPort;
        break;
    }
    return port;
}

void EthSettingsSec::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case setRmIpAdress:
            m_sRMIPAdr = m_pXMLReader->getValue(key);
            break;
        case setProtobufServerPort:
            m_nServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setRmPort:
            m_nRMPort = m_pXMLReader->getValue(key).toInt();
            break;
        }
    }
}
