#ifndef MOCKI2CCTRLCLAMPSTATUS_H
#define MOCKI2CCTRLCLAMPSTATUS_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlClampStatus : public AbstractI2cCtrlClampStatus
{
public:
    MockI2cCtrlClampStatus();
    ZeraMControllerIo::atmelRM readClampStatus(quint16& stat) override;
};

#endif // MOCKI2CCTRLCLAMPSTATUS_H
