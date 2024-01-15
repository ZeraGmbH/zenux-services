#include "test_adj_regression_scpi_query_format_mt310s2.h"
#include "proxy.h"
#include "mocki2ceepromiofactory.h"
#include "mockeeprom24lc.h"
#include "scpisingletransactionblocked.h"
#include "mockfactorycontroller.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_adj_regression_scpi_query_format_mt310s2);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_adj_regression_scpi_query_format_mt310s2::initTestCase()
{
    MockI2cEEpromIoFactory::enableMock();
    setupServers();

    QString filenameShort = ":/import_scpi_format";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_adj_regression_scpi_query_format_mt310s2::init()
{
    MockEEprom24LC::cleanAll();
}

void test_adj_regression_scpi_query_format_mt310s2::queryGainCorrectionSingleGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJGAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:ADJGAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:ADJGAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));
}

void test_adj_regression_scpi_query_format_mt310s2::queryGainCorrectionSingle()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJGAIN?", "1");
    QCOMPARE(adjustedValue, "1.11111e+07");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:ADJGAIN?", "1");
    QCOMPARE(adjustedValue, "0.111111");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:ADJGAIN?", "1");
    QCOMPARE(adjustedValue, "11.1111");
}

void test_adj_regression_scpi_query_format_mt310s2::queryGainCorrectionTotalGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:GAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:GAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:GAIN?", "1");
    qInfo("%s", qPrintable(adjustedValue));
}

void test_adj_regression_scpi_query_format_mt310s2::queryGainCorrectionTotal()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:GAIN?", "1");
    QCOMPARE(adjustedValue, "1.11111e+07");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:GAIN?", "1");
    QCOMPARE(adjustedValue, "0.111111");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:GAIN?", "1");
    QCOMPARE(adjustedValue, "11.1111");
}

void test_adj_regression_scpi_query_format_mt310s2::queryPhaseCorrectionSingleGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));
}

void test_adj_regression_scpi_query_format_mt310s2::queryPhaseCorrectionSingle()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJPHASE?", "1");
    QCOMPARE(adjustedValue, "2.22222e+07");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:ADJPHASE?", "1");
    QCOMPARE(adjustedValue, "0.222222");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:ADJPHASE?", "1");
    QCOMPARE(adjustedValue, "22.2222");
}

void test_adj_regression_scpi_query_format_mt310s2::queryPhaseCorrectionTotalGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:ADJPHASE?", "1");
    qInfo("%s", qPrintable(adjustedValue));
}

void test_adj_regression_scpi_query_format_mt310s2::queryPhaseCorrectionTotal()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:PHASE?", "1");
    QCOMPARE(adjustedValue, "2.22222e+07");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:PHASE?", "1");
    QCOMPARE(adjustedValue, "0.222222");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:PHASE?", "1");
    QCOMPARE(adjustedValue, "22.2222");
}

void test_adj_regression_scpi_query_format_mt310s2::queryOffsetCorrectionSingleGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJOFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:ADJOFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:ADJOFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));
}

void test_adj_regression_scpi_query_format_mt310s2::queryOffsetCorrectionSingle()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJOFFSET?", "1");
    QCOMPARE(adjustedValue, "3.33333e+07");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:ADJOFFSET?", "1");
    QCOMPARE(adjustedValue, "0.333333");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:ADJOFFSET?", "1");
    QCOMPARE(adjustedValue, "33.3333");
}

void test_adj_regression_scpi_query_format_mt310s2::queryOffsetCorrectionTotalGen()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:OFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:OFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:OFFSET?", "1");
    qInfo("%s", qPrintable(adjustedValue));
}

void test_adj_regression_scpi_query_format_mt310s2::queryOffsetCorrectionTotal()
{
    QString adjustedValue;
    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:OFFSET?", "1");
    QCOMPARE(adjustedValue, "3.33333e+07");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:OFFSET?", "1");
    QCOMPARE(adjustedValue, "0.333333");

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:OFFSET?", "1");
    QCOMPARE(adjustedValue, "33.3333");
}

void test_adj_regression_scpi_query_format_mt310s2::queryGainCoefficients()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:250V:CORR:GAIN:COEFFICIENT:0?");
    QCOMPARE(coefficient, "1.11111e+07");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:250V:CORR:GAIN:COEFFICIENT:0?");
    QCOMPARE(coefficient, "0.111111");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:250V:CORR:GAIN:COEFFICIENT:0?");
    QCOMPARE(coefficient, "11.1111");
}

void test_adj_regression_scpi_query_format_mt310s2::queryGainNodes()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:250V:CORR:GAIN:NODE:0?");
    QCOMPARE(coefficient, "44444444.444444;55555555.555556;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:250V:CORR:GAIN:NODE:0?");
    QCOMPARE(coefficient, "0.444444;0.555556;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:250V:CORR:GAIN:NODE:0?");
    QCOMPARE(coefficient, "44.444444;55.555556;");
}

void test_adj_regression_scpi_query_format_mt310s2::queryPhaseCoefficients()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:250V:CORR:PHASE:COEFFICIENT:0?");
    QCOMPARE(coefficient, "2.22222e+07");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:250V:CORR:PHASE:COEFFICIENT:0?");
    QCOMPARE(coefficient, "0.222222");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:250V:CORR:PHASE:COEFFICIENT:0?");
    QCOMPARE(coefficient, "22.2222");
}

void test_adj_regression_scpi_query_format_mt310s2::queryPhaseNodes()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:250V:CORR:PHASE:NODE:0?");
    QCOMPARE(coefficient, "66666666.666667;77777777.777778;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:250V:CORR:PHASE:NODE:0?");
    QCOMPARE(coefficient, "0.666667;0.777778;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:250V:CORR:PHASE:NODE:0?");
    QCOMPARE(coefficient, "66.666667;77.777778;");
}

void test_adj_regression_scpi_query_format_mt310s2::queryOffsetCoefficients()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:250V:CORR:OFFSET:COEFFICIENT:0?");
    QCOMPARE(coefficient, "3.33333e+07");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:250V:CORR:OFFSET:COEFFICIENT:0?");
    QCOMPARE(coefficient, "0.333333");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:250V:CORR:OFFSET:COEFFICIENT:0?");
    QCOMPARE(coefficient, "33.3333");
}

void test_adj_regression_scpi_query_format_mt310s2::queryOffsetNodes()
{
    QString coefficient;
    coefficient = ScpiSingleTransactionBlocked::query("SENS:m0:250V:CORR:OFFSET:NODE:0?");
    QCOMPARE(coefficient, "88888888.888889;100000000.000000;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m1:250V:CORR:OFFSET:NODE:0?");
    QCOMPARE(coefficient, "0.888889;1.000000;");

    coefficient = ScpiSingleTransactionBlocked::query("SENS:m2:250V:CORR:OFFSET:NODE:0?");
    QCOMPARE(coefficient, "88.888889;100.000000;");
}

void test_adj_regression_scpi_query_format_mt310s2::setupServers()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceMt310s2>(std::make_shared<MockFactoryController>(true));
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
