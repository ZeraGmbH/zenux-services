#ifndef I2CCTRLCOMMONVERSIONS_H
#define I2CCTRLCOMMONVERSIONS_H

#include "abstracti2ccontrollers.h"

class I2cCtrlCommonVersions : public AbstractI2cCtrlCommonVersions
{
public:
    I2cCtrlCommonVersions(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readChannelStatus(quint8 channel, quint8& stat);
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLCOMMONVERSIONS_H
