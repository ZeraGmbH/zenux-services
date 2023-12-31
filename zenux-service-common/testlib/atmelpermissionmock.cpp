#include "atmelpermissionmock.h"
#include <timerfactoryqt.h>

ZeraMControllerIoTemplate::atmelRM AtmelPermissionMock::getEEPROMAccessEnable(bool &enable)
{
    enable = m_enable;
    return ZeraMControllerIo::cmddone;
}

void AtmelPermissionMock::accessEnableAfter(int timeoutMs)
{
    m_accessTimer = TimerFactoryQt::createSingleShot(timeoutMs);
    connect(m_accessTimer.get(), &TimerTemplateQt::sigExpired, this, [&]{
        m_enable = true;
    });
    m_accessTimer->start();
}

void AtmelPermissionMock::accessDisableAfter(int timeoutMs)
{
    m_accessTimer = TimerFactoryQt::createSingleShot(timeoutMs);
    connect(m_accessTimer.get(), &TimerTemplateQt::sigExpired, this, [&]{
        m_enable = false;
    });
    m_accessTimer->start();
}
