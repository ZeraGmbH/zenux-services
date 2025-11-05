#ifndef MOCKI2CCTRLCRITICALSTATUS_H
#define MOCKI2CCTRLCRITICALSTATUS_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlCriticalStatus : public AbstractI2cCtrlCriticalStatus
{
public:
    MockI2cCtrlCriticalStatus(quint16 &criticalStatus, quint16 &criticalStatusMask);
    ZeraMControllerIo::atmelRM writeIntMask(quint16 criticalStatusMask) override;
    ZeraMControllerIo::atmelRM readIntMask(quint16& criticalStatusMask) override;
    ZeraMControllerIo::atmelRM readCriticalStatus(quint16& stat) override;
    ZeraMControllerIo::atmelRM resetCriticalStatus(quint16 stat) override;
private:
    quint16 &m_criticalStatus;
    quint16 &m_criticalStatusMask;
};

#endif // MOCKI2CCTRLCRITICALSTATUS_H
