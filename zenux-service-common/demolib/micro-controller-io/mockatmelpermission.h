#ifndef MOCKATMELPERMISSION_H
#define MOCKATMELPERMISSION_H

#include "abstracti2ccontrollers.h"
#include <timertemplateqt.h>

class MockAtmelPermission : public QObject, public AtmelPermissionTemplate
{
    Q_OBJECT
public:
    MockAtmelPermission(bool &permission);

    ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) override;

    void accessEnableAfter(int timeoutMs);
    void accessDisableAfter(int timeoutMs);
private:
    bool &m_permission;
    TimerTemplateQtPtr m_accessTimer;
};

#endif // MOCKATMELPERMISSION_H
