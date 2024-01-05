#include "test_regression_adj_scpi_query_format_com5003.h"
#include "proxy.h"
#include "i2cflashiofactoryfortest.h"
#include "flash24lc256mock.h"
#include "scpisingletransactionblocked.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_scpi_query_format_com5003);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_regression_adj_scpi_query_format_com5003::initTestCase()
{
    // permission tests are done in test_regression_adj_import_export_xml_<device>
    m_permissionMock = AtmelPermissionMock::createAlwaysEnabled();
}

void test_regression_adj_scpi_query_format_com5003::init()
{
    Flash24LC256Mock::cleanAll();
    I2cFlashIoFactoryForTest::enableMockFlash();
    setupServers(m_permissionMock.get());

    QString filenameShort = ":/import_scpi_format";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_scpi_query_format_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_scpi_query_format_com5003::queryGainCorrectionSingle()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJGAIN?", "1");
    QCOMPARE(adjustedValue, "11111111.11111111");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:ADJGAIN?", "1");
    QCOMPARE(adjustedValue, "0.11111111");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:ADJGAIN?", "1");
    QCOMPARE(adjustedValue, "11.11111111");
}

void test_regression_adj_scpi_query_format_com5003::queryGainCorrectionTotal()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:GAIN?", "1");
    QCOMPARE(adjustedValue, "11111111.11111111");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:GAIN?", "1");
    QCOMPARE(adjustedValue, "0.11111111");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:GAIN?", "1");
    QCOMPARE(adjustedValue, "11.11111111");
}

void test_regression_adj_scpi_query_format_com5003::queryPhaseCorrectionSingle()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJPHASE?", "1");
    QCOMPARE(adjustedValue, "22222222.2222222");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:ADJPHASE?", "1");
    QCOMPARE(adjustedValue, "0.2222222");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:ADJPHASE?", "1");
    QCOMPARE(adjustedValue, "22.2222222");
}

void test_regression_adj_scpi_query_format_com5003::queryPhaseCorrectionTotal()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:PHASE?", "1");
    QCOMPARE(adjustedValue, "22222222.2222222");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:PHASE?", "1");
    QCOMPARE(adjustedValue, "0.2222222");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:PHASE?", "1");
    QCOMPARE(adjustedValue, "22.2222222");
}

void test_regression_adj_scpi_query_format_com5003::queryOffsetCorrectionSingle()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJOFFSET?", "1");
    QCOMPARE(adjustedValue, "33333333.33333333");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:ADJOFFSET?", "1");
    QCOMPARE(adjustedValue, "0.33333333");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:ADJOFFSET?", "1");
    QCOMPARE(adjustedValue, "33.33333333");
}

void test_regression_adj_scpi_query_format_com5003::queryOffsetCorrectionTotal()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:OFFSET?", "1");
    QCOMPARE(adjustedValue, "33333333.33333333");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:OFFSET?", "1");
    QCOMPARE(adjustedValue, "0.33333333");
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:OFFSET?", "1");
    QCOMPARE(adjustedValue, "33.33333333");
}


void test_regression_adj_scpi_query_format_com5003::setupServers(AtmelPermissionTemplate *permissionQueryHandler)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceCom5003>(permissionQueryHandler);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
