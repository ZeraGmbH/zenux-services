#ifndef I2CCTRLCOMMONINFOCPU5975_H
#define I2CCTRLCOMMONINFOCPU5975_H

#include "abstractalli2ccontrollers.h"
#include <i2cmultiplexerfactory.h>

class I2cCtrlCommonInfoCpu5975 : public AbstractI2cCtrlCommonInfo
{
public:
    I2cCtrlCommonInfoCpu5975(QString deviceNodeName, quint8 i2cAddress,
                          quint8 i2cAddressMux, quint8 muxChannel,
                          quint8 debugLevel);
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
private:
    ZeraMControllerIo m_ctrlIo;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // I2CCTRLCOMMONINFOCPU5975_H
