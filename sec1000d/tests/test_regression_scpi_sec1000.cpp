#include "test_regression_scpi_sec1000.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include <xmldocumentcompare.h>
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_regression_scpi_sec1000);

void test_regression_scpi_sec1000::initTestCase_data()
{
    QTest::addColumn<int>("ecUnitCount");
    QTest::addColumn<QString>("dumpFile");
    QTest::newRow("COM5003") << cSEC1000dServer::Com5003EcUnitCount << "://scpi-dump-com5003.xml";
    QTest::newRow("MTxxxs2") << cSEC1000dServer::Mtxxxs2EcUnitCount << "://scpi-dump-mtxxxs2.xml";
}

void test_regression_scpi_sec1000::init()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    QFETCH_GLOBAL(int, ecUnitCount);
    m_server = std::make_unique<MockSec1000d>(tcpNetworkFactory, ecUnitCount);
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

void test_regression_scpi_sec1000::dumpScpi()
{
    QFETCH_GLOBAL(QString, dumpFile);
    QString expected = TestLogHelpers::loadFile(dumpFile);
    QString dumped = ScpiSingleTransactionBlocked::query("SYSTEM:INTERFACE:READ?", 6305);
    XmlDocumentCompare compare;
    bool ok = compare.compareXml(dumped, expected);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiff(expected, dumped);
    QVERIFY(ok);
}
