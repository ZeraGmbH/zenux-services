#ifndef MOCKI2CCTRLEEPROMPERMISSION_H
#define MOCKI2CCTRLEEPROMPERMISSION_H

#include "abstracti2ccontrollers.h"
#include <timertemplateqt.h>

class MockI2cCtrlEepromPermission : public QObject, public AbstractI2cCtrlEepromPermission
{
    Q_OBJECT
public:
    MockI2cCtrlEepromPermission(bool &permission);

    ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) override;

    void accessEnableAfter(int timeoutMs);
    void accessDisableAfter(int timeoutMs);
private:
    bool &m_permission;
    TimerTemplateQtPtr m_accessTimer;
};

#endif // MOCKI2CCTRLEEPROMPERMISSION_H
