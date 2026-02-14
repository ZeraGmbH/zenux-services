#include "accumulatorsettings.h"

enum configstate
{
    cfgAvail
};

AccumulatorSettings::AccumulatorSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["serviceconfig:interface:accumulator:avail"] = cfgAvail;
}

AccumulatorSettings::~AccumulatorSettings()
{
}

bool AccumulatorSettings::isAvailable()
{
    return m_bAvail;
}

void AccumulatorSettings::configXMLInfo(const QString &key)
{
    if (m_ConfigXMLMap.contains(key)) {
        if (m_ConfigXMLMap[key] == cfgAvail)
            m_bAvail = (m_pXMLReader->getValue(key) == "true");
    }
}
