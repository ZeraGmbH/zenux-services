#include <QVariant>
#include <xmlconfigreader.h>

#include "com5003dglobal.h"
#include "ctrlsettings.h"

cCtrlSettings::cCtrlSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["com5003dconfig:connectivity:ctrl:device:node"] = CTRLSettings::setCTRLDevNode;
    m_sDeviceNode = defaultCTRLDeviceNode;
}


QString& cCtrlSettings::getDeviceNode()
{
    return m_sDeviceNode;
}


void cCtrlSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key))
    {
        m_sDeviceNode = m_pXMLReader->getValue(key);
    }
}

