#ifndef I2CCTRLMT650E_H
#define I2CCTRLMT650E_H

#include "abstractalli2ccontrollers.h"
#include <i2cmultiplexerfactory.h>

class I2cCtrlMt650e : public AbstractI2cCtrlMt650e
{
public:
    I2cCtrlMt650e(QString deviceNodeName, quint8 i2cAddress, quint8 i2cAddressMux, quint8 muxChannel, quint8 debugLevel);
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
private:
    ZeraMControllerIo m_ctrlIo;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // I2CCTRLMT650E_H
