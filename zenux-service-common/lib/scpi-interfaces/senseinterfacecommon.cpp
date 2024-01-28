#include "senseinterfacecommon.h"
#include <i2cmultiplexerfactory.h>

QString SenseInterfaceCommon::m_version = "V1.00";

SenseInterfaceCommon::SenseInterfaceCommon(cSCPI *scpiInterface,
                                           I2cSettings *i2cSettings,
                                           SystemInfo *systemInfo,
                                           AbstractFactoryI2cCtrlPtr ctrlFactory) :
    cResource(scpiInterface),
    AdjustmentEeprom(i2cSettings->getDeviceNode(),
                     i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                     I2cMultiplexerFactory::createNullMuxer()),
    m_systemInfo(systemInfo),
    m_ctrlFactory(ctrlFactory)
{
}
