#ifndef I2CSETTINGS_H
#define I2CSETTINGS_H

#include <xmlsettings.h>
#include <QString>

namespace i2cSettings
{
enum member
{
    relaisCtrlI2cAddress,
    sysCtrlI2cAddress,
    emobCtrlI2cAddress,
    muxerI2cAddress,
    flashlI2cAddress,
    clampFlashI2cAddress
};

enum configstate
{
    SetDevNode,

    SetAtmelAdr,
    SetAtmelSysAdr,
    SetAtmelEmob,
    SetFlashMuxAdr,
    SetFlashAdr,
    SetClampFlashAdr
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
    quint8 m_nAtmelAdr;
    quint8 m_nAtmelSysAdr;
    quint8 m_nAtmelEmob;
    quint8 m_nFlashMuxAdr;
    quint8 m_nFlashAdr;
    quint8 m_nClampFlashAdr;
};


#endif // I2CSETTINGS_H
