#include "i2csettings.h"
#include <eepromi2cdeviceinterface.h>
#include <xmlconfigreader.h>

I2cSettings::I2cSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;

    m_ConfigXMLMap["serviceconfig:connectivity:i2c:debuglevel"] = i2cSettings::SetDebugLevel;
    m_ConfigXMLMap["serviceconfig:connectivity:i2c:device:node"] = i2cSettings::SetDevNode;
    m_ConfigXMLMap["serviceconfig:connectivity:i2c:eeprom_capacity"] = i2cSettings::SetEepromCapacity;
    m_ConfigXMLMap["serviceconfig:connectivity:i2c:adress:atmel"] = i2cSettings::SetAtmelAdr;
    m_ConfigXMLMap["serviceconfig:connectivity:i2c:adress:atmelsys"] = i2cSettings::SetAtmelSysAdr;
    m_ConfigXMLMap["serviceconfig:connectivity:i2c:adress:atmelemob"] = i2cSettings::SetAtmelEmob;
    m_ConfigXMLMap["serviceconfig:connectivity:i2c:adress:clampmux"] = i2cSettings::SetFlashMuxAdr;
    m_ConfigXMLMap["serviceconfig:connectivity:i2c:adress:flash"] = i2cSettings::SetFlashAdr;
    m_ConfigXMLMap["serviceconfig:connectivity:i2c:adress:clampflash"] = i2cSettings::SetClampFlashAdr;
}

int I2cSettings::getDebugLevel()
{
    return m_debugLevel;
}

quint8 I2cSettings::getI2CAdress(i2cSettings::I2cDeviceAdrTypes deviceType)
{
    quint8 r;
    switch (deviceType)
    {
    case i2cSettings::relaisCtrlI2cAddress:
        r = m_nAtmelAdr;
        break;
    case i2cSettings::sysCtrlI2cAddress:
        r = m_nAtmelSysAdr;
        break;
    case i2cSettings::emobCtrlI2cAddress:
        r = m_nAtmelEmob;
        break;
    case i2cSettings::muxerI2cAddress:
        r = m_nFlashMuxAdr;
        break;
    case i2cSettings::flashlI2cAddress:
        r = m_nFlashAdr;
        break;
    case i2cSettings::clampFlashI2cAddress:
        r = m_nClampFlashAdr;
        break;
    }
    return r;
}


QString& I2cSettings::getDeviceNode()
{
    return m_sDeviceNode;
}

int I2cSettings::getEepromByteSize()
{
    return m_eepromCapacity;
}

void I2cSettings::configXMLInfo(QString key)
{
    if (m_pXMLReader && m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case i2cSettings::SetDebugLevel:
            m_debugLevel = m_pXMLReader->getValue(key).toInt();
            break;
        case i2cSettings::SetDevNode:
            m_sDeviceNode = m_pXMLReader->getValue(key);
            break;
        case i2cSettings::SetEepromCapacity:
            m_eepromCapacity = m_pXMLReader->getValue(key).toInt();
            break;
        case i2cSettings::SetAtmelSysAdr:
            m_nAtmelSysAdr = m_pXMLReader->getValue(key).toInt();
            break;
        case i2cSettings::SetAtmelEmob:
            m_nAtmelEmob = m_pXMLReader->getValue(key).toInt();
            break;
        case i2cSettings::SetAtmelAdr:
            m_nAtmelAdr = m_pXMLReader->getValue(key).toInt();
            break;
        case i2cSettings::SetFlashMuxAdr:
            m_nFlashMuxAdr = m_pXMLReader->getValue(key).toInt();
            break;
        case i2cSettings::SetFlashAdr:
            m_nFlashAdr = m_pXMLReader->getValue(key).toInt();
            break;
        case i2cSettings::SetClampFlashAdr:
            m_nClampFlashAdr = m_pXMLReader->getValue(key).toInt();
            break;
        }
    }
}
