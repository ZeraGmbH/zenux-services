#include "test_regression_scpi_zdsp1d.h"
#include "testfactorydevicenodedsp.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include <timemachineobject.h>
#include <tcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_regression_scpi_zdsp1d);

void test_regression_scpi_zdsp1d::init()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_server = std::make_unique<MockZdsp1d>(std::make_shared<TestFactoryDeviceNodeDsp>(), tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6310, tcpNetworkFactory);
    m_dspIFace = std::make_unique<Zera::cDSPInterface>();
    m_dspIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_zdsp1d::cleanup()
{
    m_dspIFace = nullptr;
    m_proxyClient = nullptr;
    m_server = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_zdsp1d::serverUp()
{
    QString ret = ScpiSingleTransactionBlocked::query("SYSTEM:VERSION:SERVER?", 6310);
    QCOMPARE(ret, "zdsp1d V1.11");
}
