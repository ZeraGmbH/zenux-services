#ifndef TEST_SCHNUBBEL_H
#define TEST_SCHNUBBEL_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_schnubbel : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void initTestCase_data();
    void init();
    void cleanup();

    void schnubbelPollWorking();
private:
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_SCHNUBBEL_H
