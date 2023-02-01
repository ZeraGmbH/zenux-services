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
    void findScpiObject();
    void scpiEmptyNotifier();
private:
    std::unique_ptr<PCBTestServer> m_pcbServerTest;
};

#endif // TEST_AUTHORIZATIONNOTIFIER_H
