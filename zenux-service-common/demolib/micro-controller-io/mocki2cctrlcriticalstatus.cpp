#include "mocki2cctrlcriticalstatus.h"

MockI2cCtrlCriticalStatus::MockI2cCtrlCriticalStatus(quint16 &criticalStatus, quint16 &criticalStatusMask) :
    m_criticalStatus(criticalStatus),
    m_criticalStatusMask(criticalStatusMask)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCriticalStatus::writeIntMask(quint16 criticalStatusMask)
{
    m_criticalStatusMask = criticalStatusMask;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCriticalStatus::readIntMask(quint16 &criticalStatusMask)
{
    criticalStatusMask = m_criticalStatusMask;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCriticalStatus::readCriticalStatus(quint16 &criticalStatus)
{
    criticalStatus = m_criticalStatus;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCriticalStatus::resetCriticalStatus(quint16 stat)
{
    m_criticalStatus = 0;
    return ZeraMControllerIo::atmelRM::cmddone;
}
