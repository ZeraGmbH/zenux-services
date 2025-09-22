#ifndef I2CCTRLEMOB_H
#define I2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"
#include <i2cmultiplexerfactory.h>

class I2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    I2cCtrlEMOB(QString deviceNodeName, quint8 i2cAddress,
                quint8 i2cAddressMux, quint8 muxChannel,
                quint8 debugLevel);
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
    ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status) override;
private:
    ZeraMControllerIo m_ctrlIo;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // I2CCTRLEMOB_H
