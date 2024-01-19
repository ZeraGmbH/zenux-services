#include "testi2csettings.h"

TestI2cSettings::TestI2cSettings(Zera::XMLConfig::cReader *xmlread) :
    I2cSettings(xmlread)
{
}

void TestI2cSettings::setI2cAddressesEmob(QString i2cDeviceNode, quint8 i2cAdrCtrlEmob, quint8 i2cAdrMuxEmob)
{
    m_sDeviceNode = i2cDeviceNode;
    m_nAtmelEmob = i2cAdrCtrlEmob;
    m_nFlashMuxAdr = i2cAdrMuxEmob;
}
