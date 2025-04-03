#include "test_regression_scpi_mt310s2.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include <xmldocumentcompare.h>
#include <testloghelpers.h>
#include <mockeeprom24lc.h>
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_regression_scpi_mt310s2);

void test_regression_scpi_mt310s2::initTestCase_data()
{
    QTest::addColumn<QString>("serviceNameForAlternateDevice");
    QTest::newRow("MT310s2") << QString("mt310s2d");
    QTest::newRow("MT581s2") << QString("mt581s2d");
}

void test_regression_scpi_mt310s2::init()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    QFETCH_GLOBAL(QString, serviceNameForAlternateDevice);
    m_server = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, serviceNameForAlternateDevice);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_mt310s2::cleanup()
{
    MockEEprom24LC::mockCleanAll();
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_server = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_mt310s2::serverUp()
{
    QString ret = ScpiSingleTransactionBlocked::query("SYSTEM:VERSION:SERVER?");
    QCOMPARE(ret, "V42.0");
}

void test_regression_scpi_mt310s2::dumpScpi()
{
    QFETCH_GLOBAL(QString, serviceNameForAlternateDevice);
    QString dumpFile = QString(":/%1/scpi-dump.xml").arg(serviceNameForAlternateDevice);
    QString expected = TestLogHelpers::loadFile(dumpFile);
    QString dumped = ScpiSingleTransactionBlocked::query("SYSTEM:INTERFACE:READ?");
    XmlDocumentCompare compare;
    bool ok = compare.compareXml(dumped, expected);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiff(expected, dumped);
    QVERIFY(ok);
}
