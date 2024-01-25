#include "test_regression_critical_status_mt310s2.h"
#include "testfactoryi2cctrlcriticalstatus.h"
#include "scpisingletransactionblocked.h"
#include "proxy.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_regression_critical_status_mt310s2);

void test_regression_critical_status_mt310s2::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_critical_status_mt310s2::receiveOnAllFlagsSet()
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
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m6:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m7:STATUS?");
    QCOMPARE(critStat, "1");

    QString err = ScpiSingleTransactionBlocked::query("SENSE:m8:STATUS?");
    QCOMPARE(err, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_critical_status_mt310s2::resetM3()
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
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m6:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m7:STATUS?");
    QCOMPARE(critStat, "1");
}

void test_regression_critical_status_mt310s2::resetM4()
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
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m6:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m7:STATUS?");
    QCOMPARE(critStat, "1");
}

void test_regression_critical_status_mt310s2::resetM5()
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
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m6:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m7:STATUS?");
    QCOMPARE(critStat, "1");
}

void test_regression_critical_status_mt310s2::resetM7()
{
    setupServers(0xFFFF);
    QString ret = ScpiSingleTransactionBlocked::cmd("SENSE:m7:STATUS:RESET", ""); // what the f***
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
    QCOMPARE(critStat, "1");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m6:STATUS?");
    QCOMPARE(critStat, "0");
    critStat = ScpiSingleTransactionBlocked::query("SENSE:m7:STATUS?");
    QCOMPARE(critStat, "0");
}

void test_regression_critical_status_mt310s2::setupServers(quint16 initialCriticalStatus)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrlCriticalStatus>(initialCriticalStatus));
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
