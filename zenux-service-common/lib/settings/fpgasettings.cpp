#include "fpgasettings.h"

enum configstate
{
    setFPGADevNode
};

FPGASettings::FPGASettings(Zera::XMLConfig::cReader *xmlread, QString topXmlNode)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap[topXmlNode + ":connectivity:fpga:device:node"] = setFPGADevNode;
}

QString& FPGASettings::getDeviceNode()
{
    return m_sDeviceNode;
}

void FPGASettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key))
        m_sDeviceNode = m_pXMLReader->getValue(key);
}
