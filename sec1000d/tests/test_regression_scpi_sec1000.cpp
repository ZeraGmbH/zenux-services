#include "test_regression_scpi_sec1000.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include <timemachineobject.h>
#include <tcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_regression_scpi_sec1000);

void test_regression_scpi_sec1000::init()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_server = std::make_unique<MockSec1000d>(tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6305, tcpNetworkFactory);
    m_secIFace = std::make_unique<Zera::cSECInterface>();
    m_secIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_sec1000::cleanup()
{
    m_secIFace = nullptr;
    m_proxyClient = nullptr;
    m_server = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_sec1000::serverUp()
{
    QString ret = ScpiSingleTransactionBlocked::query("SYSTEM:VERSION:SERVER?", 6305);
    QCOMPARE(ret, "V42.0");
}
