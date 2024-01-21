#ifndef I2CCTRLEEPROMPERMISSION_H
#define I2CCTRLEEPROMPERMISSION_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlEepromPermission : public AbstractI2cCtrlEepromPermission
{
public:
    I2cCtrlEepromPermission(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLEEPROMPERMISSION_H
