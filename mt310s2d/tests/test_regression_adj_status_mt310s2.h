#ifndef TEST_REGRESSION_ADJ_STATUS_MT310S2_H
#define TEST_REGRESSION_ADJ_STATUS_MT310S2_H

#include "testserverforsenseinterfacemt310s2.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_status_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void statusDefaultMachine();
    void statusAllAdjusted();

    void statusClampsI1DefaultMachine();
    void statusClampsI1DefaultCL120A();
private:
    void setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory);

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_ADJ_STATUS_MT310S2_H
