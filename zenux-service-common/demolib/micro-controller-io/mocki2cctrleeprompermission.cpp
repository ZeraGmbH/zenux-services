#include "mocki2cctrleeprompermission.h"
#include <timerfactoryqt.h>

MockI2cCtrlEepromPermission::MockI2cCtrlEepromPermission(bool &permission) :
    m_permission(permission)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEepromPermission::getEEPROMAccessEnable(bool &enable)
{
    enable = m_permission;
    return ZeraMControllerIo::cmddone;
}

void MockI2cCtrlEepromPermission::accessEnableAfter(int timeoutMs)
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

void MockI2cCtrlEepromPermission::accessDisableAfter(int timeoutMs)
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
