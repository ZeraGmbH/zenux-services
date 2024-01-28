#include "senseinterfacecommon.h"
#include <i2cmultiplexerfactory.h>

SenseInterfaceCommon::SenseInterfaceCommon(cSCPI *scpiInterface, I2cSettings *i2cSettings) :
    cResource(scpiInterface),
    AdjustmentEeprom(i2cSettings->getDeviceNode(),
                     i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                     I2cMultiplexerFactory::createNullMuxer())
{
}
