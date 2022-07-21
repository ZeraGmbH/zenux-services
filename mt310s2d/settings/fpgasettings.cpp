#include "fpgasettings.h"
#include "mt310s2dglobal.h"
#include <xmlconfigreader.h>

cFPGASettings::cFPGASettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["mt310s2dconfig:connectivity:fpga:device:node"] = FPGASettings::setFPGADevNode;
    m_sDeviceNode = defaultFPGADeviceNode;
}

QString& cFPGASettings::getDeviceNode()
{
    return m_sDeviceNode;
}

void cFPGASettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        m_sDeviceNode = m_pXMLReader->getValue(key);
    }
}
