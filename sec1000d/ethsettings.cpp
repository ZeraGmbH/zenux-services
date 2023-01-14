#include "ethsettings.h"
#include <xmlconfigreader.h>

EthSettingsSec::EthSettingsSec(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["sec1000dconfig:connectivity:ethernet:ipadress:resourcemanager"] = setRMIPAdress;
    m_ConfigXMLMap["sec1000dconfig:connectivity:ethernet:port:server"] = setServerPort;
    m_ConfigXMLMap["sec1000dconfig:connectivity:ethernet:port:resourcemanager"] = setRMPort;
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
    case server:
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
        case setRMIPAdress:
            m_sRMIPAdr = m_pXMLReader->getValue(key);
            break;
        case setServerPort:
            m_nServerPort = m_pXMLReader->getValue(key).toInt();
            break;
        case setRMPort:
            m_nRMPort = m_pXMLReader->getValue(key).toInt();
            break;
        }
    }
}
