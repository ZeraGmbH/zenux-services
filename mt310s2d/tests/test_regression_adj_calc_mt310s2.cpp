#include "test_regression_adj_calc_mt310s2.h"
#include "clampfactorytest.h"
#include "clamp.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "mocki2ceepromiofactory.h"
#include "mockeeprom24lc.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_calc_mt310s2);

void test_regression_adj_calc_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
    setupServers();

    QString filenameShort = ":/import_internal";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));

    m_valueFormatter = AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter();
}

void test_regression_adj_calc_mt310s2::cleanup()
{
    MockEEprom24LC::mockCleanAll();
    m_testServer->removeAllClamps();
}

static constexpr double val = 2.0;

static constexpr double gainXmlCoeff0 = 2;
static constexpr double gainXmlCoeff1 = 3;
static constexpr double gainXmlCoeff2 = 4;

void test_regression_adj_calc_mt310s2::gainAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJGAIN?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, gainXmlCoeff0, gainXmlCoeff1, gainXmlCoeff2));
}

void test_regression_adj_calc_mt310s2::gainAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJGAIN?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, gainXmlCoeff0, gainXmlCoeff1, gainXmlCoeff2));
}

static constexpr double phaseXmlCoeff0 = 3;
static constexpr double phaseXmlCoeff1 = 4;
static constexpr double phaseXmlCoeff2 = 5;

void test_regression_adj_calc_mt310s2::phaseAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJPHASE?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, phaseXmlCoeff0, phaseXmlCoeff1, phaseXmlCoeff2));
}

void test_regression_adj_calc_mt310s2::phaseAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:PHASE?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, phaseXmlCoeff0, phaseXmlCoeff1, phaseXmlCoeff2));
}

static constexpr double offsetXmlCoeff0 = 4;
static constexpr double offsetXmlCoeff1 = 5;
static constexpr double offsetXmlCoeff2 = 6;

void test_regression_adj_calc_mt310s2::offsetAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJOFFSET?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, offsetXmlCoeff0, offsetXmlCoeff1, offsetXmlCoeff2));
}

void test_regression_adj_calc_mt310s2::offsetAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:OFFSET?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, offsetXmlCoeff0, offsetXmlCoeff1, offsetXmlCoeff2));
}

// clamps

static constexpr double cvRatioCL200ADC1000VDC_C200A = 500;

static constexpr double gainClampXmlCoeff0 = 5;
static constexpr double gainClampXmlCoeff1 = 6;
static constexpr double gainClampXmlCoeff2 = 7;

void test_regression_adj_calc_mt310s2::gainAdjValueSingleClamp()
{
    addClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:ADJGAIN?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, gainClampXmlCoeff0, gainClampXmlCoeff1, gainClampXmlCoeff2));
}

void test_regression_adj_calc_mt310s2::gainAdjValueTotalClamp()
{
    addClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:GAIN?", QString::number(val));
    double adjSingle = calcAdjValue(val, gainClampXmlCoeff0, gainClampXmlCoeff1, gainClampXmlCoeff2);
    double adjIntern = calcAdjValue(val/cvRatioCL200ADC1000VDC_C200A, gainXmlCoeff0, gainXmlCoeff1, gainXmlCoeff2);
    QString adjTotal = m_valueFormatter->m_gainScpiFormatter(adjSingle*adjIntern);
    QCOMPARE(adjustedValue, adjTotal);
}

static constexpr double phaseClampXmlCoeff0 = 6;
static constexpr double phaseClampXmlCoeff1 = 7;
static constexpr double phaseClampXmlCoeff2 = 8;

void test_regression_adj_calc_mt310s2::phaseAdjValueSingleClamp()
{
    addClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:ADJPHASE?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, phaseClampXmlCoeff0, phaseClampXmlCoeff1, phaseClampXmlCoeff2));
}

void test_regression_adj_calc_mt310s2::phaseAdjValueTotalClamp()
{
    addClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:PHASE?", QString::number(val));
    double adjSingle = calcAdjValue(val, phaseClampXmlCoeff0, phaseClampXmlCoeff1, phaseClampXmlCoeff2);
    double adjIntern = calcAdjValue(val, phaseXmlCoeff0, phaseXmlCoeff1, phaseXmlCoeff2);
    QString adjTotal = m_valueFormatter->m_phaseScpiFormatter(adjSingle+adjIntern);
    QCOMPARE(adjustedValue, adjTotal);
}

static constexpr double offsetClampXmlCoeff0 = 7;
static constexpr double offsetClampXmlCoeff1 = 8;
static constexpr double offsetClampXmlCoeff2 = 9;

void test_regression_adj_calc_mt310s2::offsetAdjValueSingleClamp()
{
    addClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:ADJOFFSET?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, offsetClampXmlCoeff0, offsetClampXmlCoeff1, offsetClampXmlCoeff2));
}

void test_regression_adj_calc_mt310s2::offsetAdjValueTotalClamp()
{
    addClamp();
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m3:C200A:CORR:OFFSET?", QString::number(val));
    double adjSingle = calcAdjValue(val, offsetClampXmlCoeff0, offsetClampXmlCoeff1, offsetClampXmlCoeff2);
    double adjIntern = calcAdjValue(val/cvRatioCL200ADC1000VDC_C200A, offsetXmlCoeff0, offsetXmlCoeff1, offsetXmlCoeff2);
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

void test_regression_adj_calc_mt310s2::addClamp()
{
    m_testServer->addClamp(cClamp::CL200ADC1000VDC, "IL1");
    QString xml = readFile(":/import_clamp.xml");
    QVERIFY(!xml.isEmpty());
    QCOMPARE(m_testServer->getClampInterface()->importClampXmls(xml, false), ZSCPI::scpiAnswer[ZSCPI::ack]);
}

QString test_regression_adj_calc_mt310s2::readFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QString();
}

double test_regression_adj_calc_mt310s2::calcAdjValue(double value, double coeff0, double coeff1, double coeff2)
{
    return coeff0 + value*coeff1 + value*value*coeff2;
}
