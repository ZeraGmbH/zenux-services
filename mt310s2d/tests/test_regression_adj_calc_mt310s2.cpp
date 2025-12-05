#include "test_regression_adj_calc_mt310s2.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "mockeepromdevice.h"
#include "scpisingletransactionblocked.h"
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_calc_mt310s2);

void test_regression_adj_calc_mt310s2::initTestCase()
{
    setupServers();

    QString filenameShort = ":/import_internal";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));

    m_valueFormatter = AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter();
}

void test_regression_adj_calc_mt310s2::cleanup()
{
    MockEepromDevice::cleanAll();
    m_testServer->removeAllClamps();
}

static constexpr double val = 2.0;

void test_regression_adj_calc_mt310s2::gainAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJGAIN?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcGainValue(val));
}

void test_regression_adj_calc_mt310s2::gainAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJGAIN?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcGainValue(val));
}

void test_regression_adj_calc_mt310s2::phaseAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJPHASE?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcPhaseValue(val));
}

void test_regression_adj_calc_mt310s2::phaseAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:PHASE?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcPhaseValue(val));
}

void test_regression_adj_calc_mt310s2::offsetAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJOFFSET?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcOffsetValue(val));
}

void test_regression_adj_calc_mt310s2::offsetAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:OFFSET?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcOffsetValue(val));
}

// clamps

void test_regression_adj_calc_mt310s2::gainAdjValueSingleClamp()
{
    m_testServer->addStdClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:ADJGAIN?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcGainValueClamp(val));
}

void test_regression_adj_calc_mt310s2::gainAdjValueTotalClamp()
{
    m_testServer->addStdClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:GAIN?", QString::number(val));
    double adjSingle = m_testServer->calcGainValueClamp(val);
    double adjIntern = m_testServer->calcGainValue(val / m_testServer->getCvRatioC200A());
    QString adjTotal = m_valueFormatter->m_gainScpiFormatter(adjSingle*adjIntern);
    QCOMPARE(adjustedValue, adjTotal);
}

void test_regression_adj_calc_mt310s2::phaseAdjValueSingleClamp()
{
    m_testServer->addStdClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:ADJPHASE?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcPhaseValueClamp(val));
}

void test_regression_adj_calc_mt310s2::phaseAdjValueTotalClamp()
{
    m_testServer->addStdClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:PHASE?", QString::number(val));
    double adjSingle = m_testServer->calcPhaseValueClamp(val);
    double adjIntern = m_testServer->calcPhaseValue(val);
    QString adjTotal = m_valueFormatter->m_phaseScpiFormatter(adjSingle+adjIntern);
    QCOMPARE(adjustedValue, adjTotal);
}

void test_regression_adj_calc_mt310s2::offsetAdjValueSingleClamp()
{
    m_testServer->addStdClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:ADJOFFSET?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), m_testServer->calcOffsetValueClamp(val));
}

void test_regression_adj_calc_mt310s2::offsetAdjValueTotalClamp()
{
    m_testServer->addStdClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:OFFSET?", QString::number(val));
    double adjSingle = m_testServer->calcOffsetValueClamp(val);
    double adjIntern = m_testServer->calcOffsetValue(val / m_testServer->getCvRatioC200A());
    QString adjTotal = m_valueFormatter->m_offsetScpiFormatter(adjSingle+adjIntern);
    QCOMPARE(adjustedValue, adjTotal);
}

void test_regression_adj_calc_mt310s2::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(
        std::make_shared<TestFactoryI2cCtrl>(true),
        tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
