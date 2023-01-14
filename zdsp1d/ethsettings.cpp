#include "ethsettings.h"
#include <xmlconfigreader.h>

EthSettingsDsp::EthSettingsDsp(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:ipadress:resourcemanager"] = setRMIPAdress;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:port:protobufserver"] = setprotobufServerPort;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:port:scpiserver"] = setscpiServerPort;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:port:resourcemanager"] = setRMPort;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:scpiactive"] = setSCPIactive;
}

QString EthSettingsDsp::getRMIPadr()
{
    return m_sRMIPAdr;
}

quint16 EthSettingsDsp::getPort(ethmember member)
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

bool EthSettingsDsp::isSCPIactive()
{
    return m_bSCPIactive;
}

void EthSettingsDsp::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case setRMIPAdress:
            m_sRMIPAdr = m_pXMLReader->getValue(key);
            break;
        case setprotobufServerPort:
            m_nProtobufServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setscpiServerPort:
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

