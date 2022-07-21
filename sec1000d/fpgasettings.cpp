#include <QVariant>
#include <xmlconfigreader.h>

#include "sec1000dglobal.h"
#include "fpgasettings.h"


cFPGASettings::cFPGASettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["sec1000dconfig:connectivity:fpga:device:node"] = FPGASettings::setFPGADevNode;
    m_sDeviceNode = defaultFPGADeviceNode;
}


QString& cFPGASettings::getDeviceNode()
{
    return m_sDeviceNode;
}


void cFPGASettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key))
    {
        m_sDeviceNode = m_pXMLReader->getValue(key);
    }
}
