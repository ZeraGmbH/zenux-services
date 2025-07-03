#ifndef I2CCTRLREADERROR_H
#define I2CCTRLREADERROR_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlReadError : public AbstractI2cCtrlReadError
{
    Q_OBJECT
public:
    I2cCtrlReadError(QString deviceNodeName, quint8 i2cAddressSystemCtrl, quint8 i2cAddressRelayCtrl, quint8 i2cAddressEmobCtrl, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readErrorSystemCtrl(QString &errorReply) override;
    ZeraMControllerIo::atmelRM readErrorRelayCtrl(QString &errorReply) override;
    ZeraMControllerIo::atmelRM readErrorEmobCtrl(QString &errorReply) override;

private:
    ZeraMControllerIo m_ctrlIoSystemCtrl;
    ZeraMControllerIo m_ctrlIoRelayCtrl;
    ZeraMControllerIo m_ctrlIoEmobCtrl;
};

#endif // I2CCTRLREADERROR_H
