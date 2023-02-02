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

    void findPCBServerScpiObject();
    void findStatusInterfaceScpiObject();

    void executeAuthorizationQuery();
    void executeRegisterNotifier();
    void getNotiferIdAndAuthoStatus();
private:
    std::unique_ptr<PCBTestServer> m_pcbServerTest;
};

#endif // TEST_AUTHORIZATIONNOTIFIER_H
