#ifndef TESTI2CCTRLCOMMONINFO_H
#define TESTI2CCTRLCOMMONINFO_H

#include "abstracti2ccontrollers.h"

class TestI2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    TestI2cCtrlCommonInfo(QString ctrlTypeName);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
private:
    QString m_prefix;
};

#endif // TESTI2CCTRLCOMMONINFO_H
