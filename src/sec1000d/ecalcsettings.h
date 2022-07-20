#ifndef ECALCSETTINGS_H
#define ECALCSETTINGS_H

#include <xmlsettings.h>

namespace ECalculatorSystem
{

enum configstate
{
    setnumber,
    setbaseadress,
    setirqadress
};
}

class cECalculatorSettings : public XMLSettings
{
    Q_OBJECT
public:
    cECalculatorSettings(Zera::XMLConfig::cReader *xmlread);
    virtual ~cECalculatorSettings();
    quint16 getNumber(); // here we ask for the number of error calculator units
    quint32 getBaseAdress(); // our base adress
    quint32 getIrqAdress(); // our base adress

public slots:
    virtual void configXMLInfo(QString key);

private:
    quint16 m_nECalcUnits;
    quint32 m_nECalcUnitBaseAddress;
    quint32 m_nECalcUnitIrqAdress;
};


#endif // ECALCSETTINGS_H
