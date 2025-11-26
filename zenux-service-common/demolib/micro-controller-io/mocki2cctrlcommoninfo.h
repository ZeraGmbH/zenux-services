#ifndef MOCKI2CCTRLCOMMONINFO_H
#define MOCKI2CCTRLCOMMONINFO_H

#include "abstractalli2ccontrollers.h"
#include "abstractfactoryi2cctrl.h"

class MockI2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    MockI2cCtrlCommonInfo(AbstractFactoryI2cCtrl::ControllerTypes ctrlType, quint8 muxChannel, const QString &ctrlTypeNamePrefix = "");
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
private:
    QString m_prefix;
    quint8 m_muxChannel;
};

#endif // MOCKI2CCTRLCOMMONINFO_H
