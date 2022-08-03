#include "mt310s2dglobal.h"
#include "ctrlsettings.h"
#include <xmlconfigreader.h>

cCtrlSettings::cCtrlSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["mt310s2dconfig:connectivity:ctrl:device:node"] = CTRLSettings::setCTRLDevNode;
    m_sDeviceNode = defaultCTRLDeviceNode;
}

QString& cCtrlSettings::getDeviceNode()
{
    return m_sDeviceNode;
}

void cCtrlSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        m_sDeviceNode = m_pXMLReader->getValue(key);
    }
}
