#ifndef I2CSETTINGS_H
#define I2CSETTINGS_H

#include <xmlsettings.h>

namespace i2cSettings
{
enum member
{
    master,
    atmel,
    flash
};

enum configstate
{
    SetDevNode,
    SetMasterAdr,
    SetAtmelAdr,
    SetFlashAdr
};
}

class cI2CSettings : public XMLSettings
{
    Q_OBJECT

public:
    cI2CSettings(Zera::XMLConfig::cReader *xmlread);
    quint8 getI2CAdress(i2cSettings::member member);
    QString& getDeviceNode();

public slots:
    virtual void configXMLInfo(QString key);

private:
    QString m_sDeviceNode;
    quint8 m_nMasterAdr, m_nAtmelAdr, m_nFlashAdr;
};

#endif // I2CSETTINGS_H
