#include "accumulatorsettings.h"

enum configstate
{
    cfgAvail
};

accumulatorSettings::accumulatorSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["serviceconfig:interface:accumulator:avail"] = cfgAvail;
}

accumulatorSettings::~accumulatorSettings()
{
}

bool accumulatorSettings::isAvailable()
{
    return m_bAvail;
}

void accumulatorSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        if (m_ConfigXMLMap[key] == cfgAvail)
            m_bAvail = (m_pXMLReader->getValue(key) == "true");
    }
}
