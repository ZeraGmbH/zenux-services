#ifndef TESTI2CCTRLEEPROMPERMISSION_H
#define TESTI2CCTRLEEPROMPERMISSION_H

#include "abstractalli2ccontrollers.h"
#include <timertemplateqt.h>

class TestI2cCtrlEepromPermission : public QObject, public AbstractI2cCtrlEepromPermission
{
    Q_OBJECT
public:
    TestI2cCtrlEepromPermission(bool &permission);

    ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) override;

    void accessEnableAfter(int timeoutMs);
    void accessDisableAfter(int timeoutMs);
private:
    bool &m_permission;
    TimerTemplateQtPtr m_accessTimer;
};

#endif // TESTI2CCTRLEEPROMPERMISSION_H
