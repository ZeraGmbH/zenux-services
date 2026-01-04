#include "mockclampplugger.h"
#include "mockeepromdevice.h"
#include "clamp.h"
#include <QFile>

MockClampPlugger::MockClampPlugger(cSenseSettingsPtr senseSettings, I2cSettingsPtr i2cSettings) :
    m_senseSettings(senseSettings),
    m_i2cSettings(i2cSettings)
{
}

QByteArray MockClampPlugger::getFreshEEpromData(int clampType)
{
    Q_INIT_RESOURCE(clamps);
    if (cClamp::isValidClampType(clampType)) {
        const QString clampName = cClamp::getClampTypeName(clampType);
        QFile eepromFile(":/clamps/fresh/" + clampName + ".eeprom");
        if (eepromFile.open(QFile::ReadOnly))
            return eepromFile.readAll();
    }
    return QByteArray();
}

void MockClampPlugger::initClampAdjMem(int clampType, QString channelAlias)
{
    const QByteArray clampData = getFreshEEpromData(clampType);
    QByteArray eepromData(AbstractEepromI2cDevice::capacity24LC256, 0xFF);
    for (int byte=0; byte<clampData.size(); ++byte)
        eepromData[byte] = clampData[byte];

    SenseSystem::cChannelSettings *channelSettingClamp = m_senseSettings->findChannelSettingByAlias1(channelAlias);
    Q_ASSERT(channelSettingClamp);
    MockEepromDevice::setData({m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::clampFlashI2cAddress)},
                              eepromData,
                              {m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress)},
                              channelSettingClamp->m_nMuxChannelNo);
}
