#ifndef TEST_HARDWARE_OVERLOAD_H
#define TEST_HARDWARE_OVERLOAD_H

#include "resmanrunfacade.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_hardware_overload : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void initTestCase_data();
    void init();
    void cleanup();

    void initialOverloadsZero();
    void initialOverloadsAllOn();

private:
    void setupServersAndClient();
    bool hasAuxChannels(const QString &serviceName);

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<QObject> m_server;

    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_HARDWARE_OVERLOAD_H
