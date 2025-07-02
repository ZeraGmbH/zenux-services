#ifndef I2CCTRLREADERROR_H
#define I2CCTRLREADERROR_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlReadError : public AbstractI2cCtrlReadError
{
public:
    I2cCtrlReadError(QString deviceNodeName, quint8 i2cAddressSysCtrl, quint8 i2cAddressRelCtrl, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readErrorSysCtrl(QString &errorReply) override;
    ZeraMControllerIo::atmelRM readErrorRelCtrl(QString &errorReply) override;

private:
    ZeraMControllerIo m_ctrlIoSysCtrl;
    ZeraMControllerIo m_ctrlIoRelCtrl;
};


#endif // I2CCTRLREADERROR_H
