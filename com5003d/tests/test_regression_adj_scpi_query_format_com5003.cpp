#include "test_regression_adj_scpi_query_format_com5003.h"
#include "mockatmelctrlfactory.h"
#include "proxy.h"
#include "i2cflashiofactoryfortest.h"
#include "eeprom24lcmock.h"
#include "scpisingletransactionblocked.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_scpi_query_format_com5003);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_regression_adj_scpi_query_format_com5003::initTestCase()
{
    I2cFlashIoFactoryForTest::enableMockFlash();
    setupServers();

    QString filenameShort = ":/import_scpi_format";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_scpi_query_format_com5003::init()
{
    EEprom24LCMock::cleanAll();
}

void test_regression_adj_scpi_query_format_com5003::queryGainCorrectionSingleGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJGAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:ADJGAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:ADJGAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));
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

void test_regression_adj_scpi_query_format_com5003::queryGainCorrectionTotalGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:GAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:GAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:GAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));
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

void test_regression_adj_scpi_query_format_com5003::queryPhaseCorrectionSingleGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));
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

void test_regression_adj_scpi_query_format_com5003::queryPhaseCorrectionTotalGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:PHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:PHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:PHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));
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

void test_regression_adj_scpi_query_format_com5003::queryOffsetCorrectionSingleGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJOFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:ADJOFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:ADJOFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));
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

void test_regression_adj_scpi_query_format_com5003::queryOffsetCorrectionTotalGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:OFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:240V:CORR:OFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:240V:CORR:OFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));
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

void test_regression_adj_scpi_query_format_com5003::queryGainCoefficients()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:240V:CORR:GAIN:COEFFICIENT:0?");
    QCOMPARE(coefficient, "1.11111e+07");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:240V:CORR:GAIN:COEFFICIENT:0?");
    QCOMPARE(coefficient, "0.111111");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:240V:CORR:GAIN:COEFFICIENT:0?");
    QCOMPARE(coefficient, "11.1111");
}

void test_regression_adj_scpi_query_format_com5003::queryGainNodes()
{
    // Interesting results: 44444444.44444445 / trailing ';'
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:240V:CORR:GAIN:NODE:0?");
    QCOMPARE(coefficient, "44444444.44444445;55555555.55555555;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:240V:CORR:GAIN:NODE:0?");
    QCOMPARE(coefficient, "0.44444444;0.55555556;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:240V:CORR:GAIN:NODE:0?");
    QCOMPARE(coefficient, "44.44444444;55.55555556;");
}

void test_regression_adj_scpi_query_format_com5003::queryPhaseCoefficients()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:240V:CORR:PHASE:COEFFICIENT:0?");
    QCOMPARE(coefficient, "2.22222e+07");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:240V:CORR:PHASE:COEFFICIENT:0?");
    QCOMPARE(coefficient, "0.222222");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:240V:CORR:PHASE:COEFFICIENT:0?");
    QCOMPARE(coefficient, "22.2222");
}

void test_regression_adj_scpi_query_format_com5003::queryPhaseNodes()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:240V:CORR:PHASE:NODE:0?");
    QCOMPARE(coefficient, "66666666.66666666;77777777.77777778;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:240V:CORR:PHASE:NODE:0?");
    QCOMPARE(coefficient, "0.66666667;0.77777778;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:240V:CORR:PHASE:NODE:0?");
    QCOMPARE(coefficient, "66.66666667;77.77777778;");
}

void test_regression_adj_scpi_query_format_com5003::queryOffsetCoefficients()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:240V:CORR:OFFSET:COEFFICIENT:0?");
    QCOMPARE(coefficient, "3.33333e+07");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:240V:CORR:OFFSET:COEFFICIENT:0?");
    QCOMPARE(coefficient, "0.333333");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:240V:CORR:OFFSET:COEFFICIENT:0?");
    QCOMPARE(coefficient, "33.3333");
}

void test_regression_adj_scpi_query_format_com5003::queryOffsetNodes()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:240V:CORR:OFFSET:NODE:0?");
    QCOMPARE(coefficient, "88888888.88888890;100000000.00000000;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:240V:CORR:OFFSET:NODE:0?");
    QCOMPARE(coefficient, "0.88888889;1.00000000;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:240V:CORR:OFFSET:NODE:0?");
    QCOMPARE(coefficient, "88.88888889;100.00000000;");
}

void test_regression_adj_scpi_query_format_com5003::setupServers()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceCom5003>(std::make_shared<MockAtmelCtrlFactory>(true));
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
