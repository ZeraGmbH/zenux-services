#ifndef TEST_MOCKSERVICE_MT310S2D_FULL_H
#define TEST_MOCKSERVICE_MT310S2D_FULL_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_mockservice_mt310s2d_full : public QObject
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
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_MOCKSERVICE_MT310S2D_FULL_H
