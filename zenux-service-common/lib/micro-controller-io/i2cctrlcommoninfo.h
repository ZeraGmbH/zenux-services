#ifndef I2CCTRLCOMMONINFO_H
#define I2CCTRLCOMMONINFO_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    I2cCtrlCommonInfo(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLCOMMONINFO_H
