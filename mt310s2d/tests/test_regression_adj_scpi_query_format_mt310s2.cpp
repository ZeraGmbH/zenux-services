#include "test_regression_adj_scpi_query_format_mt310s2.h"
#include "proxy.h"
#include "i2cflashiofactoryfortest.h"
#include "flash24lc256mock.h"
#include "scpisingletransactionblocked.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_scpi_query_format_mt310s2);

static const QDateTime refTime = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

void test_regression_adj_scpi_query_format_mt310s2::initTestCase()
{
    // permission tests are done in test_regression_adj_import_export_xml_<device>
    m_permissionMock = AtmelPermissionMock::createAlwaysEnabled();
}

void test_regression_adj_scpi_query_format_mt310s2::init()
{
    Flash24LC256Mock::cleanAll();
    I2cFlashIoFactoryForTest::enableMockFlash();
    setupServers(m_permissionMock.get());

    QString filenameShort = ":/import_scpi_format";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_scpi_query_format_mt310s2::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_scpi_query_format_mt310s2::queryGainCorrectionSingle()
{
    QString adjustedValue;

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:ADJG?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:ADJG?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    adjustedValue = ScpiSingleTransactionBlocked::cmd("SENS:m2:250V:CORR:ADJG?", "1");
    qInfo("%s", qPrintable(adjustedValue));

    // no assertions yet - we are investigating....
    //QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_adj_scpi_query_format_mt310s2::setupServers(AtmelPermissionTemplate *permissionQueryHandler)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceMt310s2>(permissionQueryHandler);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
