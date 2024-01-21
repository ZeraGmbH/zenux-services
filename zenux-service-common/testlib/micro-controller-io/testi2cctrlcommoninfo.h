#ifndef TESTI2CCTRLCOMMONINFO_H
#define TESTI2CCTRLCOMMONINFO_H

#include "abstractalli2ccontrollers.h"

class TestI2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
};

#endif // TESTI2CCTRLCOMMONINFO_H
