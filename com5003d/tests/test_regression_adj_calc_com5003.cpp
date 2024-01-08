#include "test_regression_adj_calc_com5003.h"
#include "proxy.h"
#include "i2cflashiofactoryfortest.h"
#include "flash24lc256mock.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_calc_com5003);

void test_regression_adj_calc_com5003::initTestCase()
{
    // permission tests are done in test_regression_adj_import_export_xml_<device>
    m_permissionMock = AtmelPermissionMock::createAlwaysEnabled();
    I2cFlashIoFactoryForTest::enableMockFlash();
    setupServers(m_permissionMock.get());

    QString filenameShort = ":/import_internal";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));

    m_valueFormatter = AdjustScpiValueFormatterFactory::createCom5003AdjFormatter();
}

void test_regression_adj_calc_com5003::cleanup()
{
    Flash24LC256Mock::cleanAll();
}

static constexpr double val = 2.0;

static constexpr double gainXmlCoeff0 = 2;
static constexpr double gainXmlCoeff1 = 3;
static constexpr double gainXmlCoeff2 = 4;

void test_regression_adj_calc_com5003::gainAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJGAIN?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, gainXmlCoeff0, gainXmlCoeff1, gainXmlCoeff2));
}

void test_regression_adj_calc_com5003::gainAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJGAIN?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, gainXmlCoeff0, gainXmlCoeff1, gainXmlCoeff2));
}

static constexpr double phaseXmlCoeff0 = 3;
static constexpr double phaseXmlCoeff1 = 4;
static constexpr double phaseXmlCoeff2 = 5;

void test_regression_adj_calc_com5003::phaseAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJPHASE?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, phaseXmlCoeff0, phaseXmlCoeff1, phaseXmlCoeff2));
}

void test_regression_adj_calc_com5003::phaseAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:PHASE?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, phaseXmlCoeff0, phaseXmlCoeff1, phaseXmlCoeff2));
}

static constexpr double offsetXmlCoeff0 = 4;
static constexpr double offsetXmlCoeff1 = 5;
static constexpr double offsetXmlCoeff2 = 6;

void test_regression_adj_calc_com5003::offsetAdjValueSingle()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:ADJOFFSET?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, offsetXmlCoeff0, offsetXmlCoeff1, offsetXmlCoeff2));
}

void test_regression_adj_calc_com5003::offsetAdjValueTotal()
{
    QString adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:240V:CORR:OFFSET?", QString::number(val));
    QCOMPARE(adjustedValue.toDouble(), calcAdjValue(val, offsetXmlCoeff0, offsetXmlCoeff1, offsetXmlCoeff2));
}

void test_regression_adj_calc_com5003::setupServers(AtmelPermissionTemplate *permissionQueryHandler)
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

double test_regression_adj_calc_com5003::calcAdjValue(double value, double coeff0, double coeff1, double coeff2)
{
    return coeff0 + value*coeff1 + value*value*coeff2;
}
