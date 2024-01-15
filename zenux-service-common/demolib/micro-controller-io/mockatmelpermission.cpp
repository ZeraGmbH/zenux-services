#include "mockatmelpermission.h"
#include <timerfactoryqt.h>

MockAtmelPermission::MockAtmelPermission(bool &permission) :
    m_permission(permission)
{
}

ZeraMControllerIoTemplate::atmelRM MockAtmelPermission::getEEPROMAccessEnable(bool &enable)
{
    enable = m_permission;
    return ZeraMControllerIo::cmddone;
}

void MockAtmelPermission::accessEnableAfter(int timeoutMs)
{
    if(timeoutMs == 0) {
        m_permission = true;
        return;
    }
    m_accessTimer = TimerFactoryQt::createSingleShot(timeoutMs);
    connect(m_accessTimer.get(), &TimerTemplateQt::sigExpired, this, [&]{
        m_permission = true;
    });
    m_accessTimer->start();
}

void MockAtmelPermission::accessDisableAfter(int timeoutMs)
{
    if(timeoutMs == 0) {
        m_permission = false;
        return;
    }
    m_accessTimer = TimerFactoryQt::createSingleShot(timeoutMs);
    connect(m_accessTimer.get(), &TimerTemplateQt::sigExpired, this, [&]{
        m_permission = false;
    });
    m_accessTimer->start();
}
