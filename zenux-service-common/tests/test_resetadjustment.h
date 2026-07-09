#ifndef TEST_RESETADJUSTMENT_H
#define TEST_RESETADJUSTMENT_H

#include "testserverforsenseinterfacemt310s2.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_resetAdjustment : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void resetAdjDataOk();

private:
    void setupServers();

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    Zera::ProxyClientPtr m_proxyClient;

};

#endif // TEST_RESETADJUSTMENT_H
