#ifndef TESTI2CCTRLCOMMONINFOFOO_H
#define TESTI2CCTRLCOMMONINFOFOO_H

#include "abstracti2ccontrollers.h"

class TestI2cCtrlCommonInfoFoo : public AbstractI2cCtrlCommonInfo
{
public:
    TestI2cCtrlCommonInfoFoo(QString ctrlTypeName);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
private:
    QString m_prefix;
};

#endif // TESTI2CCTRLCOMMONINFOFOO_H
