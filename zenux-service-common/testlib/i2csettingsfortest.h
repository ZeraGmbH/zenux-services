#ifndef I2CSETTINGSFORTEST_H
#define I2CSETTINGSFORTEST_H

#include "i2csettings.h"

class I2cSettingsForTest : public I2cSettings
{
public:
    I2cSettingsForTest(Zera::XMLConfig::cReader *xmlread);
    void setI2cAddressesEmob(QString i2cDeviceNode, quint8 i2cAdrCtrlEmob, quint8 i2cAdrMuxEmob);
};

#endif // I2CSETTINGSFORTEST_H
