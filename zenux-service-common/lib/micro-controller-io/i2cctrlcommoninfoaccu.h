#ifndef I2CCTRLCOMMONINFOACCU_H
#define I2CCTRLCOMMONINFOACCU_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlCommonInfoAccu : public AbstractI2cCtrlCommonInfo
{
public:
    I2cCtrlCommonInfoAccu(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
private:
    ZeraMControllerIo m_ctrlIo; // we tunnel through systemcontroller!!
};

#endif // I2CCTRLCOMMONINFOACCU_H
