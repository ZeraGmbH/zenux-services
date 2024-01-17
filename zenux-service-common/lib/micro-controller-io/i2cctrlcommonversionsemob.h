#ifndef I2CCTRLCOMMONVERSIONSEMOB_H
#define I2CCTRLCOMMONVERSIONSEMOB_H

#include "abstracti2ccontrollers.h"
#include <i2cmultiplexerfactory.h>

class I2cCtrlCommonVersionsEmob : public AbstractI2cCtrlCommonVersions
{
public:
    I2cCtrlCommonVersionsEmob(QString deviceNodeName, quint8 i2cAddress,
                              quint8 i2cAddressMux, quint8 muxChannel,
                              quint8 debugLevel);
    ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
private:
    ZeraMControllerIo m_ctrlIo;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // I2CCTRLCOMMONVERSIONSEMOB_H
