#ifndef TEST_SERVERUNREGISTERNOTIFIER_H
#define TEST_SERVERUNREGISTERNOTIFIER_H

#include "mockatmel.h"
#include "pcbtestserver.h"
#include "adjustmentstatusnull.h"
#include <memory>
#include <QObject>

class test_serverunregisternotifier : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void oneScpiConnection();
private:
    void registerNotifier(QString inputCmd);
    void unregisterNotifier();
    std::unique_ptr<PCBTestServer> m_pcbServerTest;
    MockAtmel *m_atmel;
    AdjustmentStatusNull *m_adjustmentStatusNull;

};

#endif // TEST_SERVERUNREGISTERNOTIFIER_H
