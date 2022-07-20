#include "i2csettings.h"
#include "mt310s2dglobal.h"
#include <xmlconfigreader.h>
#include <i2cmuxernull.h>

cI2CSettings::cI2CSettings(Zera::XMLConfig::cReader *xmlread)
{
    m_pXMLReader = xmlread;
    m_ConfigXMLMap["mt310s2dconfig:connectivity:i2c:device:node"] = i2cSettings::SetDevNode;
    m_ConfigXMLMap["mt310s2dconfig:connectivity:i2c-systemctrl:adress:atmel"] = i2cSettings::SetAtmelSysAdr;
    m_ConfigXMLMap["mt310s2dconfig:connectivity:i2c:adress:atmel"] = i2cSettings::SetAtmelAdr;
    m_ConfigXMLMap["mt310s2dconfig:connectivity:i2c:adress:clampmux"] = i2cSettings::SetFlashMuxAdr;
    m_ConfigXMLMap["mt310s2dconfig:connectivity:i2c:adress:flash"] = i2cSettings::SetFlashAdr;
    m_ConfigXMLMap["mt310s2dconfig:connectivity:i2c:adress:clampflash"] = i2cSettings::SetClampFlashAdr;
    m_sDeviceNode = defaultI2CDeviceNode;
    m_nAtmelSysAdr = defaultI2CSysAtmelAdress;
    m_nAtmelAdr = defaultI2CAtmelAdress;
    m_nFlashMuxAdr = defaultI2CFlashMuxAdress;
    m_nFlashAdr = defaultI2CFlashAdress;
    m_nClampFlashAdr = defaultI2CClampFlashAdr;
}

quint8 cI2CSettings::getI2CAdress(i2cSettings::member member)
{
    quint8 r;
    switch (member)
    {
    case i2cSettings::atmelsys:
        r = m_nAtmelSysAdr;
        break;
    case i2cSettings::atmel:
        r = m_nAtmelAdr;
        break;
    case i2cSettings::flashmux:
        r = m_nFlashMuxAdr;
        break;
    case i2cSettings::flash:
        r = m_nFlashAdr;
        break;
    case i2cSettings::clampflash:
        r = m_nClampFlashAdr;
        break;
    }
    return r;
}


QString& cI2CSettings::getDeviceNode()
{
    return m_sDeviceNode;
}

I2cMuxerInterface::Ptr cI2CSettings::createClampMuxer(quint8 ctrlChannel)
{
    int i2cAddressMux = getI2CAdress(i2cSettings::flashmux);
    return I2cMuxerInterface::Ptr(new I2cMuxer(m_sDeviceNode, i2cAddressMux, (ctrlChannel-4) | 8, 0));
}

I2cMuxerInterface::Ptr cI2CSettings::createNullMuxer()
{
    return I2cMuxerInterface::Ptr(new I2cMuxerNull());
}

void cI2CSettings::configXMLInfo(QString key)
{
    bool ok;
    if (m_pXMLReader && m_ConfigXMLMap.contains(key)) {
        switch (m_ConfigXMLMap[key])
        {
        case i2cSettings::SetDevNode:
            m_sDeviceNode = m_pXMLReader->getValue(key);
            break;
        case i2cSettings::SetAtmelSysAdr:
            m_nAtmelSysAdr = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case i2cSettings::SetAtmelAdr:
            m_nAtmelAdr = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case i2cSettings::SetFlashMuxAdr:
            m_nFlashMuxAdr = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case i2cSettings::SetFlashAdr:
            m_nFlashAdr = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        case i2cSettings::SetClampFlashAdr:
            m_nClampFlashAdr = m_pXMLReader->getValue(key).toInt(&ok);
            break;
        }
    }
}
