#ifndef ATMELPERMISSIONMOCK_H
#define ATMELPERMISSIONMOCK_H

#include <timertemplateqt.h>
#include <atmelpermissiontemplate.h>

class AtmelPermissionMock : public QObject, public AtmelPermissionTemplate
{
    Q_OBJECT
public:
    ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) override;
    void accessEnableAfter(int timeoutMs);
    void accessDisableAfter(int timeoutMs);
private:
    bool m_enable = false;
    TimerTemplateQtPtr m_accessTimer;
};

#endif // ATMELPERMISSIONMOCK_H
