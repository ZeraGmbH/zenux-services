#ifndef MOCKATMELCRITICALSTATUS_H
#define MOCKATMELCRITICALSTATUS_H

#include "abstracti2ccontrollers.h"

class MockAtmelCriticalStatus : public AtmelCriticalStatus
{
public:
    // TODO: critical status handling / interrupts
    MockAtmelCriticalStatus(quint16 &criticalStatus, quint16 &criticalStatusMask);
    ZeraMControllerIo::atmelRM writeIntMask(quint16 criticalStatusMask) override;
    ZeraMControllerIo::atmelRM readIntMask(quint16& criticalStatusMask) override;
    ZeraMControllerIo::atmelRM readCriticalStatus(quint16& stat) override;
    ZeraMControllerIo::atmelRM resetCriticalStatus(quint16 stat) override;
private:
    quint16 &m_criticalStatus;
    quint16 &m_criticalStatusMask;
};

#endif // MOCKATMELCRITICALSTATUS_H
