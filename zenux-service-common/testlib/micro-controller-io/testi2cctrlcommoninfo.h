#ifndef TESTI2CCTRLCOMMONINFO_H
#define TESTI2CCTRLCOMMONINFO_H

#include "abstractalli2ccontrollers.h"
#include "abstractfactoryi2cctrl.h"

class TestI2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    explicit TestI2cCtrlCommonInfo(AbstractFactoryI2cCtrl::ControllerTypes ctrlType, quint8 muxChannel);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
private:
    AbstractFactoryI2cCtrl::ControllerTypes m_ctrlType;
    quint8 m_muxChannel;
};

#endif // TESTI2CCTRLCOMMONINFO_H
