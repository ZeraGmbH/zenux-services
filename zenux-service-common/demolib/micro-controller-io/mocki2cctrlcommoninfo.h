#ifndef MOCKI2CCTRLCOMMONINFO_H
#define MOCKI2CCTRLCOMMONINFO_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    MockI2cCtrlCommonInfo(QString ctrlTypeName);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) override;
private:
    QString m_prefix;
};

#endif // MOCKI2CCTRLCOMMONINFO_H
