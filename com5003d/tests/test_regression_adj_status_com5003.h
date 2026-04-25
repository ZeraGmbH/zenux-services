#ifndef TEST_REGRESSION_ADJ_STATUS_COM5003_H
#define TEST_REGRESSION_ADJ_STATUS_COM5003_H

#include "testserverforsenseinterfacecom5003.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_status_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void statusDefaultMachine();
    void statusAllAdjusted();

private:
    void setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory);

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<TestServerForSenseInterfaceCom5003> m_testServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_ADJ_STATUS_COM5003_H
