#ifndef TEST_AUTHORIZATIONNOTIFIER_H
#define TEST_AUTHORIZATIONNOTIFIER_H

#include "testpcbservernotifications.h"
#include "adjustmentstatusinterface.h"
#include <memory.h>

class test_authorizationnotifier : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void findPCBServerScpiObject();
    void findStatusInterfaceScpiObject();

    void getInitialAuthoStatus();
    void getNotiferId();

    void notifyAuthoStatusEnabled();
    void notifyAuthoStatusEnabledDisabled();

    void unregisteredNotifierAuthoStatusEnabled();
    void noNotificationAuthoStatusEnabled();
private:
    QString getAuthoStatus();
    std::unique_ptr<TestPcbServerNotifications> m_pcbServerTest;
    I2cCtrlEepromPermissionPtr m_PermissionCtrl;
    AdjustmentStatusInterface *m_adjustmentStatusNull;
};

#endif // TEST_AUTHORIZATIONNOTIFIER_H
