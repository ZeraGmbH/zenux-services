#ifndef I2CCTRLPLL_H
#define I2CCTRLPLL_H

#include "abstracti2ccontrollers.h"

class I2cCtrlPll : public AbstractI2cCtrlPll
{
public:
    I2cCtrlPll(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIoTemplate::atmelRM extracted();
    ZeraMControllerIo::atmelRM setPLLChannel(quint8 chn) override;
    ZeraMControllerIo::atmelRM readPLLChannel(quint8& chn) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLPLL_H
