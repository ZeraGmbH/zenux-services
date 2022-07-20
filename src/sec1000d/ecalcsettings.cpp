#include "ecalcsettings.h"
#include <xmlsettings.h>
#include <xmlconfigreader.h>

cECalculatorSettings::cECalculatorSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;

    m_ConfigXMLMap[QString("sec1000dconfig:resource:ecalcchannel:n")] = ECalculatorSystem::setnumber;
    m_ConfigXMLMap[QString("sec1000dconfig:resource:ecalcchannel:base")] = ECalculatorSystem::setbaseadress;
    m_ConfigXMLMap[QString("sec1000dconfig:resource:ecalcchannel:irqbase")] = ECalculatorSystem::setirqadress;
}


cECalculatorSettings::~cECalculatorSettings()
{
}


quint16 cECalculatorSettings::getNumber()
{
   return m_nECalcUnits;
}


quint32 cECalculatorSettings::getBaseAdress()
{
    return m_nECalcUnitBaseAddress;
}


quint32 cECalculatorSettings::getIrqAdress()
{
    return m_nECalcUnitIrqAdress;
}


void cECalculatorSettings::configXMLInfo(QString key)
{
    bool ok;

    if (m_ConfigXMLMap.contains(key))
    {
        switch (m_ConfigXMLMap[key])
        {
        case ECalculatorSystem::setnumber:
            m_nECalcUnits = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case ECalculatorSystem::setbaseadress:
            m_nECalcUnitBaseAddress = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case ECalculatorSystem::setirqadress:
            m_nECalcUnitIrqAdress = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        }
    }
}



