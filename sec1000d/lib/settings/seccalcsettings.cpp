#include "seccalcsettings.h"
#include <xmlconfigreader.h>

SecCalculatorSettings::SecCalculatorSettings(Zera::XMLConfig::cReader *xmlread, int ecUnitCount) :
    m_ecUnitCount(ecUnitCount)
{
    m_pXMLReader = xmlread;

    m_ConfigXMLMap[QString("serviceconfig:resource:ecalcchannel:base")] = ECalculatorSystem::setbaseadress;
    m_ConfigXMLMap[QString("serviceconfig:resource:ecalcchannel:irqbase")] = ECalculatorSystem::setirqadress;
    qInfo("Number of error calculator units: %i", m_ecUnitCount);
}

SecCalculatorSettings::~SecCalculatorSettings()
{
}

int SecCalculatorSettings::getNumber()
{
   return m_ecUnitCount;
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
        case ECalculatorSystem::setbaseadress:
            m_nECalcUnitBaseAddress = m_pXMLReader->getValue(key).toInt();
            break;
        case ECalculatorSystem::setirqadress:
            m_nECalcUnitIrqAdress = m_pXMLReader->getValue(key).toInt();
            break;
        }
    }
}
