#include "seccalcsettings.h"
#include <xmlconfigreader.h>

SecCalculatorSettings::SecCalculatorSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;

    m_ConfigXMLMap[QString("serviceconfig:resource:ecalcchannel:n")] = ECalculatorSystem::setnumber;
    m_ConfigXMLMap[QString("serviceconfig:resource:ecalcchannel:base")] = ECalculatorSystem::setbaseadress;
    m_ConfigXMLMap[QString("serviceconfig:resource:ecalcchannel:irqbase")] = ECalculatorSystem::setirqadress;
}

SecCalculatorSettings::~SecCalculatorSettings()
{
}

quint16 SecCalculatorSettings::getNumber()
{
   return m_nECalcUnits;
}

quint32 SecCalculatorSettings::getBaseAdress()
{
    return m_nECalcUnitBaseAddress;
}

quint32 SecCalculatorSettings::getIrqAdress()
{
    return m_nECalcUnitIrqAdress;
}

void SecCalculatorSettings::configXMLInfo(QString key)
{
    if (m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case ECalculatorSystem::setnumber:
            m_nECalcUnits = m_pXMLReader->getValue(key).toInt();
            break;
        case ECalculatorSystem::setbaseadress:
            m_nECalcUnitBaseAddress = m_pXMLReader->getValue(key).toInt();
            break;
        case ECalculatorSystem::setirqadress:
            m_nECalcUnitIrqAdress = m_pXMLReader->getValue(key).toInt();
            break;
        }
    }
}
