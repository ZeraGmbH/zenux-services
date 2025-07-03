#ifndef TESTI2CCTRLREADERROR_H
#define TESTI2CCTRLREADERROR_H

#include "abstractalli2ccontrollers.h"

class TestI2cCtrlReadError : public AbstractI2cCtrlReadError
{
public:
    TestI2cCtrlReadError();
    ZeraMControllerIo::atmelRM readErrorSystemCtrl(QString& answer) override;
    ZeraMControllerIo::atmelRM readErrorRelayCtrl(QString& answer) override;
    ZeraMControllerIo::atmelRM readErrorEmobCtrl(QString& answer) override;
};


#endif // TESTI2CCTRLRANGES_H
