#ifndef TEST_AUTHORIZATIONNOTIFIER_H
#define TEST_AUTHORIZATIONNOTIFIER_H

#include "pcbtestserver.h"
#include <memory.h>
#include <QObject>

class test_authorizationnotifier : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void findPCBServerScpiObject();
    void findStatusInterfaceScpiObject();

    void getInitialAuthoStatus();
    void getNotiferId();

    void authoStatusChangedToOne();
    void authoStatusChangedToZero();

    void unregisteredNotifierAuthoStatusChangedToOne();
private:
    void registerNotifier();
    void unregisterNotifier();
    QString getAuthoStatus();
    std::unique_ptr<PCBTestServer> m_pcbServerTest;
    MockAtmel *m_atmel;
};

#endif // TEST_AUTHORIZATIONNOTIFIER_H
