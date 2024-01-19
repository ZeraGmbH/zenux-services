#ifndef DEMOI2CCTRLCOMMONINFO_H
#define DEMOI2CCTRLCOMMONINFO_H

#include "abstracti2ccontrollers.h"

class DemoI2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    DemoI2cCtrlCommonInfo(QString prefix);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
private:
    QString m_prefix;
};

#endif // DEMOI2CCTRLCOMMONINFO_H
