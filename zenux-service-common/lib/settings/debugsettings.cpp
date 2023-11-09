#include "debugsettings.h"

enum debugconfigstate
{
    setdebuglevel
};

cDebugSettings::cDebugSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader=xmlread;
    m_ConfigXMLMap["serviceconfig:connectivity:debuglevel"] = setdebuglevel;
}

quint8 cDebugSettings::getDebugLevel()
{
    return m_nDebugLevel;
}

void cDebugSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key))
        m_nDebugLevel = m_pXMLReader->getValue(key).toInt();
}
