#include <QVariant>
#include <xmlconfigreader.h>

#include "debugsettings.h"


cDebugSettings::cDebugSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader=xmlread;
    m_ConfigXMLMap["sec1000dconfig:connectivity:debuglevel"] = DebugSettings::setdebuglevel;
}


quint8 cDebugSettings::getDebugLevel()
{
    return m_nDebugLevel;
}


void cDebugSettings::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        m_nDebugLevel = m_pXMLReader->getValue(key).toInt(&ok);
    }
}



