#include "atmelpermissionmock.h"
#include <timerfactoryqt.h>

AtmelPermissionTemplatePtrU AtmelPermissionMock::createAlwaysEnabled()
{
    std::unique_ptr<AtmelPermissionMock> ptr = std::make_unique<AtmelPermissionMock>();
    ptr->accessEnableAfter(0);
    return ptr;
}

AtmelPermissionTemplatePtrU AtmelPermissionMock::createAlwaysDisabled()
{
    std::unique_ptr<AtmelPermissionMock> ptr = std::make_unique<AtmelPermissionMock>();
    ptr->accessDisableAfter(0);
    return ptr;
}

ZeraMControllerIoTemplate::atmelRM AtmelPermissionMock::getEEPROMAccessEnable(bool &enable)
{
    enable = m_enable;
    return ZeraMControllerIo::cmddone;
}

void AtmelPermissionMock::accessEnableAfter(int timeoutMs)
{
    if(timeoutMs == 0) {
        m_enable = true;
        return;
    }
    m_accessTimer = TimerFactoryQt::createSingleShot(timeoutMs);
    connect(m_accessTimer.get(), &TimerTemplateQt::sigExpired, this, [&]{
        m_enable = true;
    });
    m_accessTimer->start();
}

void AtmelPermissionMock::accessDisableAfter(int timeoutMs)
{
    if(timeoutMs == 0) {
        m_enable = false;
        return;
    }
    m_accessTimer = TimerFactoryQt::createSingleShot(timeoutMs);
    connect(m_accessTimer.get(), &TimerTemplateQt::sigExpired, this, [&]{
        m_enable = false;
    });
    m_accessTimer->start();
}
