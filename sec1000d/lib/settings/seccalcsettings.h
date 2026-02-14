#ifndef SECCALCSETTINGS_H
#define SECCALCSETTINGS_H

#include <xmlsettings.h>

namespace ECalculatorSystem
{

enum configstate
{
    setbaseadress,
    setirqadress
};
}

class SecCalculatorSettings : public XMLSettings
{
    Q_OBJECT
public:
    SecCalculatorSettings(Zera::XMLConfig::cReader *xmlread, int ecUnitCount);
    virtual ~SecCalculatorSettings();
    int getNumber(); // here we ask for the number of error calculator units
    quint32 getBaseAdress(); // our base adress
    quint32 getIrqAdress(); // our base adress

public slots:
    void configXMLInfo(const QString &key) override;

private:
    int m_ecUnitCount;
    quint32 m_nECalcUnitBaseAddress;
    quint32 m_nECalcUnitIrqAdress;
};

#endif // SECCALCSETTINGS_H
