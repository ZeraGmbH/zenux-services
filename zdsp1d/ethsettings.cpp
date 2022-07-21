#include <QVariant>
#include <xmlconfigreader.h>

#include "zdspglobal.h"
#include "ethsettings.h"


cETHSettings::cETHSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:ipadress:resourcemanager"] = setRMIPAdress;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:port:protobufserver"] = setprotobufServerPort;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:port:scpiserver"] = setscpiServerPort;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:port:resourcemanager"] = setRMPort;
    m_ConfigXMLMap["zdsp1dconfig:connectivity:ethernet:scpiactive"] = setSCPIactive;
}


QString cETHSettings::getRMIPadr()
{
    return m_sRMIPAdr;
}


quint16 cETHSettings::getPort(ethmember member)
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


bool cETHSettings::isSCPIactive()
{
    return m_bSCPIactive;
}


void cETHSettings::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case setRMIPAdress:
            m_sRMIPAdr = m_pXMLReader->getValue(key);
            break;
        case setprotobufServerPort:
            m_nProtobufServerPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setscpiServerPort:
            m_nSCPIServerPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setRMPort:
            m_nRMPort = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case setSCPIactive:
            m_bSCPIactive = (m_pXMLReader->getValue(key).toInt(&ok) == 1);
            break;
        }
    }
}

