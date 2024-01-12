#include "atmelpermissionmock.h"
#include <timerfactoryqt.h>

bool AtmelPermissionMock::m_permission = false;

AtmelPermissionTemplatePtrU AtmelPermissionMock::create()
{
    return std::make_unique<AtmelPermissionMock>();
}

void AtmelPermissionMock::setPermission(bool permission)
{
    m_permission = permission;
}

ZeraMControllerIoTemplate::atmelRM AtmelPermissionMock::getEEPROMAccessEnable(bool &enable)
{
    enable = m_permission;
    return ZeraMControllerIo::cmddone;
}

void AtmelPermissionMock::accessEnableAfter(int timeoutMs)
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

void AtmelPermissionMock::accessDisableAfter(int timeoutMs)
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
