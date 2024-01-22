#include "testi2cctrleeprompermission.h"
#include <timerfactoryqt.h>

TestI2cCtrlEepromPermission::TestI2cCtrlEepromPermission(bool &permission) :
    m_permission(permission)
{
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEepromPermission::getEEPROMAccessEnable(bool &enable)
{
    enable = m_permission;
    return ZeraMControllerIo::cmddone;
}

void TestI2cCtrlEepromPermission::accessEnableAfter(int timeoutMs)
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

void TestI2cCtrlEepromPermission::accessDisableAfter(int timeoutMs)
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
