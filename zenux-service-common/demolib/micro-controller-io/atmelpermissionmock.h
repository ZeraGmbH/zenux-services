#ifndef ATMELPERMISSIONMOCK_H
#define ATMELPERMISSIONMOCK_H

#include <timertemplateqt.h>
#include "atmelcommon.h"

class AtmelPermissionMock : public QObject, public AtmelPermissionTemplate
{
    Q_OBJECT
public:
    static AtmelPermissionTemplatePtrU create();
    static void setPermission(bool permission);

    ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) override;

    void accessEnableAfter(int timeoutMs);
    void accessDisableAfter(int timeoutMs);
private:
    static bool m_permission; // static: simul singleton behavior / TOOD move behavior to MockAtmelCtrlFactory
    TimerTemplateQtPtr m_accessTimer;
};

#endif // ATMELPERMISSIONMOCK_H
