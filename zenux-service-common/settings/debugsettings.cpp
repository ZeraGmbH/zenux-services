#include "debugsettings.h"
#include <xmlconfigreader.h>

cDebugSettings::cDebugSettings(Zera::XMLConfig::cReader *xmlread, QString topXmlNode)
{
    m_pXMLReader=xmlread;
    m_ConfigXMLMap[topXmlNode + ":connectivity:debuglevel"] = DebugSettings::setdebuglevel;
}

quint8 cDebugSettings::getDebugLevel()
{
    return m_nDebugLevel;
}

void cDebugSettings::configXMLInfo(QString key)
{
    bool ok;
    if (m_ConfigXMLMap.contains(key)) {
        m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
    }
}
