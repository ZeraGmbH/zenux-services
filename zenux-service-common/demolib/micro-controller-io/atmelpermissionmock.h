#ifndef ATMELPERMISSIONMOCK_H
#define ATMELPERMISSIONMOCK_H

#include <timertemplateqt.h>
#include "atmelcommon.h"

class AtmelPermissionMock : public QObject, public AtmelPermissionTemplate
{
    Q_OBJECT
public:
    AtmelPermissionMock(bool &permission);

    ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) override;

    void accessEnableAfter(int timeoutMs);
    void accessDisableAfter(int timeoutMs);
private:
    bool &m_permission;
    TimerTemplateQtPtr m_accessTimer;
};

#endif // ATMELPERMISSIONMOCK_H
