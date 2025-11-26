#ifndef MOCKI2CCTRLCOMMONINFO_H
#define MOCKI2CCTRLCOMMONINFO_H

#include "abstractalli2ccontrollers.h"
#include "abstractfactoryi2cctrl.h"

class MockI2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    MockI2cCtrlCommonInfo(AbstractFactoryI2cCtrl::ControllerTypes ctrlType, qint8 muxChannel, const QString &commonInfoOverride);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
private:
    QString getVersionPrefix() const;
    qint8 m_muxChannel;
    AbstractFactoryI2cCtrl::ControllerTypes m_ctrlType;
    QString m_commonInfoOverride;
};

#endif // MOCKI2CCTRLCOMMONINFO_H
