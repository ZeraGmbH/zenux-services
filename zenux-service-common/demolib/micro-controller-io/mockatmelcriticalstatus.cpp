#include "mockatmelcriticalstatus.h"

MockAtmelCriticalStatus::MockAtmelCriticalStatus(quint16 &criticalStatus, quint16 &criticalStatusMask) :
    m_criticalStatus(criticalStatus),
    m_criticalStatusMask(criticalStatusMask)
{
}

ZeraMControllerIoTemplate::atmelRM MockAtmelCriticalStatus::writeIntMask(quint16 criticalStatusMask)
{
    m_criticalStatusMask = criticalStatusMask;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelCriticalStatus::readIntMask(quint16 &criticalStatusMask)
{
    criticalStatusMask = m_criticalStatusMask;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelCriticalStatus::readCriticalStatus(quint16 &criticalStatus)
{
    criticalStatus = m_criticalStatus;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelCriticalStatus::resetCriticalStatus(quint16 stat)
{
    m_criticalStatus = 0;
    return ZeraMControllerIo::atmelRM::cmddone;
}
