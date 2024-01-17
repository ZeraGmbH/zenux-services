#ifndef MOCKI2CCTRLCOMMONVERSIONS_H
#define MOCKI2CCTRLCOMMONVERSIONS_H

#include "abstracti2ccontrollers.h"

class MockI2cCtrlCommonVersions : public AbstractI2cCtrlCommonVersions
{
public:
    MockI2cCtrlCommonVersions();
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) override;
};

#endif // MOCKI2CCTRLCOMMONVERSIONS_H
