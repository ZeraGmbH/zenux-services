#include "sourcecontrolsettings.h"

enum configstate
{
    cfgCapabilitiesFile
};

SourceControlSettings::SourceControlSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["serviceconfig:resource:sourcecontrol:capabilfile"] = cfgCapabilitiesFile;
}

QString SourceControlSettings::getSourceCapFile()
{
    return m_sourceCapFile;
}

void SourceControlSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        if (m_ConfigXMLMap[key] == cfgCapabilitiesFile)
            m_sourceCapFile = m_pXMLReader->getValue(key);
    }
}
