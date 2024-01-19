#ifndef TESTI2CSETTINGS_H
#define TESTI2CSETTINGS_H

#include "i2csettings.h"

class TestI2cSettings : public I2cSettings
{
public:
    TestI2cSettings(Zera::XMLConfig::cReader *xmlread);
    void setI2cAddressesEmob(QString i2cDeviceNode, quint8 i2cAdrCtrlEmob, quint8 i2cAdrMuxEmob);
};

#endif // TESTI2CSETTINGS_H
