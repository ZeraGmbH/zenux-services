#include "accumulatorsettings.h"

enum configstate
{
    cfgAvail
};

accumulatorSettings::accumulatorSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    BatterySettings* settings = new BatterySettings;
    m_BatterySettingsList.append(settings);

    m_ConfigXMLMap["serviceconfig:scpiconnection:accumulator:avail"] = cfgAvail;

}

accumulatorSettings::~accumulatorSettings()
{
    for(auto channel : qAsConst(m_BatterySettingsList))
        delete channel;
}

QList<accumulatorSettings::BatterySettings *> &accumulatorSettings::getBatterySettings()
{
    return m_BatterySettingsList;
}

void accumulatorSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        if (m_ConfigXMLMap[key] == cfgAvail)
            m_BatterySettingsList.at(0)->m_bAvail = (m_pXMLReader->getValue(key) == "true");
    }
}
