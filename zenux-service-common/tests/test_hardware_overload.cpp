#include "test_hardware_overload.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include "mockcom5003d.h"
#include "mockmt310s2d.h"
#include "scpisingletransactionblocked.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "controllerdefinitions.h"
#include "zscpi_response_definitions.h"
#include "controllerpersitentdata.h"
#include <mockeepromdevice.h>
#include <mocktcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_hardware_overload)

void test_hardware_overload::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_hardware_overload::initTestCase_data()
{
    QTest::addColumn<QString>("serviceName"); // Simplification: mt581s2d is a mt310s2d with different config
    QTest::addColumn<QStringList>("supportOverloadMNames");
    QTest::newRow("MT310s2") << QString("mt310s2d") << QStringList();                           // MT310s2 has no overload support
    QTest::newRow("MT581s2") << QString("mt581s2d") << (QStringList() << "m3" << "m4" << "m5"); // MT581s2 has support on IL1/IL2/IL3
    QTest::newRow("COM5003") << QString("com5003d") << (QStringList() << "m3" << "m4" << "m5"); // COM5003 has support on IL1/IL2/IL3
}

void test_hardware_overload::init()
{
    ControllerPersitentData::getData().m_criticalStatus = 0;
}

void test_hardware_overload::cleanup()
{
    MockEepromDevice::cleanAll();
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_server = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_hardware_overload::initialOverloadsZero()
{
    setupServersAndClient();

    QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m0:HARDOVERLOAD?"), "0");
}

void test_hardware_overload::initialOverloadsAllOn()
{
    ControllerPersitentData::getData().m_criticalStatus =
        (1<<overloadInterrupt_I1) |
        (1<<overloadInterrupt_I2) |
        (1<<overloadInterrupt_I3) |
        (1<<overloadInterrupt_I4) |
        (1<<overloadInterrupt_U1) |
        (1<<overloadInterrupt_U2) |
        (1<<overloadInterrupt_U3) |
        (1<<overloadInterrupt_U4);
    setupServersAndClient();

    QFETCH_GLOBAL(QString, serviceName);
    QFETCH_GLOBAL(QStringList, supportOverloadMNames);
    QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m0:HARDOVERLOAD?"), supportOverloadMNames.contains("m0") ? "1" : "0");
    QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m1:HARDOVERLOAD?"), supportOverloadMNames.contains("m1") ? "1" : "0");
    QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m2:HARDOVERLOAD?"), supportOverloadMNames.contains("m2") ? "1" : "0");
    QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m3:HARDOVERLOAD?"), supportOverloadMNames.contains("m3") ? "1" : "0");
    QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m4:HARDOVERLOAD?"), supportOverloadMNames.contains("m4") ? "1" : "0");
    QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m5:HARDOVERLOAD?"), supportOverloadMNames.contains("m5") ? "1" : "0");

    if (hasAuxChannels(serviceName)) {
        QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m6:HARDOVERLOAD?"), supportOverloadMNames.contains("m6") ? "1" : "0");
        QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m7:HARDOVERLOAD?"), supportOverloadMNames.contains("m7") ? "1" : "0");
    }
    else { // COM5003 has no m6/m7 (AUX)
        QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m6:HARDOVERLOAD?"), "nak");
        QCOMPARE(ScpiSingleTransactionBlocked::query("SENSE:m7:HARDOVERLOAD?"), "nak");
    }
}

void test_hardware_overload::setupServersAndClient()
{
    QFETCH_GLOBAL(QString, serviceName);
    if (serviceName != "com5003d")
        m_server = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpNetworkFactory, serviceName);
    else
        m_server = std::make_unique<MockCom5003d>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

bool test_hardware_overload::hasAuxChannels(const QString &serviceName)
{
    return serviceName != "com5003d";
}
