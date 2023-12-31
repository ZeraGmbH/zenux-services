#include "atmelpermissionmock.h"
#include <timerfactoryqt.h>

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
