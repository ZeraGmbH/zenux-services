#include "test_regression_scpi_com5003.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include <mockeeprom24lc.h>
#include <timemachineobject.h>
#include <tcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_regression_scpi_com5003);

void test_regression_scpi_com5003::init()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_server = std::make_unique<MockCom5003d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_com5003::cleanup()
{
    MockEEprom24LC::mockCleanAll();
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_server = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_com5003::serverUp()
{
    QString ret = ScpiSingleTransactionBlocked::query("SYSTEM:VERSION:SERVER?");
    QCOMPARE(ret, "V42.0");
}
