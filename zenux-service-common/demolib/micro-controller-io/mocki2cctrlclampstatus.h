#ifndef MOCKI2CCTRLCLAMPSTATUS_H
#define MOCKI2CCTRLCLAMPSTATUS_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlClampStatus : public AbstractI2cCtrlClampStatus
{
public:
    MockI2cCtrlClampStatus(quint16 &clampConnectMask);
    ZeraMControllerIo::atmelRM readClampStatus(quint16& stat) override;
private:
    quint16 &m_clampConnectMask;
};

#endif // MOCKI2CCTRLCLAMPSTATUS_H
