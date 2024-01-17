#ifndef TEST_MOCKSERVICE_COM5003D_FULL_H
#define TEST_MOCKSERVICE_COM5003D_FULL_H

#include "resmanrunfacade.h"
#include "mockcom5003dfull.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_mockservice_com5003d_full : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void connectServer();
    void smokeTestSystemInterface();
    void smokeTestStatusInterface();
    void smokeTestSenseInterface();
    void smokeTestFoutGroupChannelResourceAndInterface();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockCom5003dFull> m_server;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_MOCKSERVICE_COM5003D_FULL_H
