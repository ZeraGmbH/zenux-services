#include "i2csettingsfortest.h"

I2cSettingsForTest::I2cSettingsForTest(Zera::XMLConfig::cReader *xmlread) :
    cI2CSettings(xmlread)
{
}

void I2cSettingsForTest::setI2cAddressesEmob(QString i2cDeviceNode, quint8 i2cAdrCtrlEmob, quint8 i2cAdrMuxEmob)
{
    m_sDeviceNode = i2cDeviceNode;
    m_nAtmelEmob = i2cAdrCtrlEmob;
    m_nFlashMuxAdr = i2cAdrMuxEmob;
}
