#include "test_regression_critical_status_com5003.h"
#include "testfactoryi2cctrlcriticalstatus.h"
#include "scpisingletransactionblocked.h"
#include "proxy.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_regression_critical_status_com5003);

void test_regression_critical_status_com5003::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_critical_status_com5003::receiveOnAllFlagsSet()
{
    setupServers(0xFFFF);
    QString critStat;
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m0:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m1:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m2:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m3:STATUS?");
    QCOMPARE(critStat, "1");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m4:STATUS?");
    QCOMPARE(critStat, "1");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m5:STATUS?");
    QCOMPARE(critStat, "1");

    QString err = ScpiSingleTransactionBlocked::query("SENSE:m6:STATUS?");
    QCOMPARE(err, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_critical_status_com5003::resetM3()
{
    setupServers(0xFFFF);
    QString ret = ScpiSingleTransactionBlocked::cmd("SENSE:m3:STATUS:RESET", ""); // what the f***
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString critStat;
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m0:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m1:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m2:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m3:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m4:STATUS?");
    QCOMPARE(critStat, "1");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m5:STATUS?");
    QCOMPARE(critStat, "1");
}

void test_regression_critical_status_com5003::resetM4()
{
    setupServers(0xFFFF);
    QString ret = ScpiSingleTransactionBlocked::cmd("SENSE:m4:STATUS:RESET", ""); // what the f***
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString critStat;
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m0:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m1:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m2:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m3:STATUS?");
    QCOMPARE(critStat, "1");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m4:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m5:STATUS?");
    QCOMPARE(critStat, "1");
}

void test_regression_critical_status_com5003::resetM5()
{
    setupServers(0xFFFF);
    QString ret = ScpiSingleTransactionBlocked::cmd("SENSE:m5:STATUS:RESET", ""); // what the f***
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString critStat;
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m0:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m1:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m2:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m3:STATUS?");
    QCOMPARE(critStat, "1");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m4:STATUS?");
    QCOMPARE(critStat, "1");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m5:STATUS?");
    QCOMPARE(critStat, "0");
}

void test_regression_critical_status_com5003::setupServers(quint16 initialCriticalStatus)
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(std::make_shared<TestFactoryI2cCtrlCriticalStatus>(initialCriticalStatus),
                                                                        tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
