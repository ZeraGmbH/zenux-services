#ifndef I2CSETTINGS_H
#define I2CSETTINGS_H

#include <xmlsettings.h>
#include <QString>
#include <memory>

namespace i2cSettings
{
enum I2cDeviceAdrTypes
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
    SetDebugLevel,
    SetDevNode,
    SetEepromCapacity,

    SetAtmelAdr,
    SetAtmelSysAdr,
    SetAtmelEmob,
    SetFlashMuxAdr,
    SetFlashAdr,
    SetClampFlashAdr
};
}

class I2cSettings : public XMLSettings
{
    Q_OBJECT
public:
    I2cSettings(Zera::XMLConfig::cReader *xmlread);
    int getDebugLevel() const;
    quint8 getI2CAdress(i2cSettings::I2cDeviceAdrTypes deviceType) const;
    const QString &getDeviceNode() const;
    int getEepromByteSize() const;
public slots:
    virtual void configXMLInfo(QString key);
protected:
    int m_debugLevel = 1;
    QString m_sDeviceNode;
    int m_eepromCapacity = 0;
    quint8 m_nAtmelAdr;
    quint8 m_nAtmelSysAdr;
    quint8 m_nAtmelEmob;
    quint8 m_nFlashMuxAdr;
    quint8 m_nFlashAdr;
    quint8 m_nClampFlashAdr;
};

typedef std::shared_ptr<I2cSettings> I2cSettingsPtr;

#endif // I2CSETTINGS_H
