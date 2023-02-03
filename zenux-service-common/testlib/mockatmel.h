#ifndef MOCKATMEL_H
#define MOCKATMEL_H

#include <timertemplateqt.h>
#include <QObject>
#include <atmelpermissiontemplate.h>

class MockAtmel : public AtmelPermissionTemplate, public QObject
{
public:
    ZeraMcontrollerBase::atmelRM getEEPROMAccessEnable(bool &enable) override;
    void accessEnableAfter(int timeoutMs);
    void accessDisableAfter(int timeoutMs);
private:
    bool m_enable = false;
    TimerTemplateQtPtr m_accessTimer;
};

#endif // MOCKATMEL_H
