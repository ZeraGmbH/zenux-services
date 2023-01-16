#include "fpgasettings.h"
#include <xmlconfigreader.h>

enum configstate
{
    setFPGADevNode
};

cFPGASettings::cFPGASettings(Zera::XMLConfig::cReader *xmlread, QString topXmlNode)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap[topXmlNode + ":connectivity:fpga:device:node"] = setFPGADevNode;
}

QString& cFPGASettings::getDeviceNode()
{
    return m_sDeviceNode;
}

void cFPGASettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key))
        m_sDeviceNode = m_pXMLReader->getValue(key);
}
