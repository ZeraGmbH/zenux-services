#ifndef DEMOI2CCTRLREADERROR_H
#define DEMOI2CCTRLREADERROR_H

#include "abstractalli2ccontrollers.h"


class DemoI2cCtrlReadError : public AbstractI2cCtrlReadError
{
public:
    DemoI2cCtrlReadError();
    ZeraMControllerIo::atmelRM readError(QString& answer) override;
};


#endif // DEMOI2CCTRLREADERROR_H
