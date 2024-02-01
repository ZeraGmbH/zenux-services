#ifndef TEST_REGRESSION_CRITICAL_STATUS_COM5003_H
#define TEST_REGRESSION_CRITICAL_STATUS_COM5003_H

#include "proxyclient.h"
#include "resmanrunfacade.h"
#include "testserverforsenseinterfacecom5003.h"

// TODO: SCPI command sit on the wrong place: This is is not a per channel thingy!!!
class test_regression_critical_status_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();
    void receiveOnAllFlagsSet();
    void resetM3();
    void resetM4();
    void resetM5();
private:
    void setupServers(quint16 initialCriticalStatus);
    std::unique_ptr<TestServerForSenseInterfaceCom5003> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_CRITICAL_STATUS_COM5003_H
