#include <timerfactoryqt.h>
#include "mockatmel.h"

ZeraMcontrollerBase::atmelRM MockAtmel::getEEPROMAccessEnable(bool &enable)
{
    enable = m_enable;
    return ZeraMcontrollerBase::cmddone;
}

void MockAtmel::accessEnableAfter(int timeoutMs)
{
    m_accessTimer = TimerFactoryQt::createSingleShot(timeoutMs);
    connect(m_accessTimer.get(), &TimerTemplateQt::sigExpired, [&]{
        m_enable = true;
    });
    m_accessTimer->start();
}

void MockAtmel::accessDisableAfter(int timeoutMs)
{
    m_accessTimer = TimerFactoryQt::createSingleShot(timeoutMs);
    connect(m_accessTimer.get(), &TimerTemplateQt::sigExpired, [&]{
        m_enable = false;
    });
    m_accessTimer->start();
}
