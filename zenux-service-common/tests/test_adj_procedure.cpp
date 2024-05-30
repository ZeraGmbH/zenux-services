#include "test_adj_procedure.h"
#include "adjflags.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "mockeeprom24lc.h"
#include "mocki2ceepromiofactory.h"
#include "mockserverparamgenerator.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include "mt310s2systeminfomock.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QFile>
#include <QTest>

QTEST_MAIN(test_adj_procedure)

void test_adj_procedure::initTestCase()
{
    MockI2cEEpromIoFactory::enableMock();
    Mt310s2SystemInfoMock::setDeviceName("MT310s2 ADW5859");
    Mt310s2SystemInfoMock::setSerialNumber("050059467");
}

void test_adj_procedure::init()
{
    MockEEprom24LC::mockCleanAll();
}

void test_adj_procedure::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_procedure::adjDeviceDataScpiReadOk()
{
    prepareEepromTakenFromAdjustedDevice();
    setupServers();

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_adj_procedure::adjDeviceDataScpiAdjusted()
{
    prepareEepromTakenFromAdjustedDevice();
    setupServers();

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::adjusted));
}

void test_adj_procedure::adjDeviceXml()
{
    prepareEepromTakenFromAdjustedDevice();
    setupServers();

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?");
    xmlExported = XmlHelperForTest::prepareForCompare(xmlExported);
    xmlExported = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExported);

    QFile xmlFile(":/mt310s2-050059467.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);
    xmlExpected = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_adj_procedure::writeGainNodesStoreStartExport()
{
    setupServers();
    setGainNodesOnTwoRanges();
    writeAdjToChip();
    simulReboot();

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?");
    xmlExported = XmlHelperForTest::prepareForCompare(xmlExported);
    xmlExported = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExported);

    QFile xmlFile(":/gain_nodes_write.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);
    xmlExpected = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_adj_procedure::writeGainNodesComputeStoreStartExport()
{
    setupServers();
    setGainNodesOnTwoRanges();
    computeAdj();
    writeAdjToChip();
    simulReboot();

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?");
    xmlExported = XmlHelperForTest::prepareForCompare(xmlExported);
    xmlExported = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExported);

    QFile xmlFile(":/gain_compute.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);
    xmlExpected = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_adj_procedure::writePhaseNodesStoreStartExport()
{
    setupServers();
    setPhaseNodesOnTwoRanges();
    writeAdjToChip();
    simulReboot();

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?");
    xmlExported = XmlHelperForTest::prepareForCompare(xmlExported);
    xmlExported = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExported);

    QFile xmlFile(":/phase_nodes_write.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);
    xmlExpected = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_adj_procedure::writePhaseNodesComputeStoreStartExport()
{
    setupServers();
    setPhaseNodesOnTwoRanges();
    computeAdj();
    writeAdjToChip();
    simulReboot();

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?");
    xmlExported = XmlHelperForTest::prepareForCompare(xmlExported);
    xmlExported = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExported);

    // Hmm: Values are slightly different from mt310s2-050059467.xml:
    // mt310s2-050059467.xml: <Coefficients>0.001351420673;-0.000045097160;0.000000000000;0.000000000000;</Coefficients>
    // phase_compute.xml:     <Coefficients>0.001351403740;-0.000045104510;0.000000000000;0.000000000000;</Coefficients>
    QFile xmlFile(":/phase_compute.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);
    xmlExpected = XmlHelperForTest::removeDeviceSpecificEntriesFromXml(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_adj_procedure::setupServers()
{
    AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true);
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);

    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(ctrlFactory, true);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_adj_procedure::prepareEepromTakenFromAdjustedDevice()
{
    std::unique_ptr<SettingsContainer> settings =  std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettings *i2cSettings = settings->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockReadFromFile(i2cSettings->getDeviceNode(),
                                             i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                             ":/mt310s2-050059467.eeprom"));
}

void test_adj_procedure::setGainNodesOnTwoRanges()
{
    // taken from mt310s2-050059467.xml
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:GAIN:NODE:0", "0.999735;199.987000"), ZSCPI::scpiAnswer[ZSCPI::ack]);
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:m0:250V:CORR:GAIN:NODE:1", "0.999703;299.979000"), ZSCPI::scpiAnswer[ZSCPI::ack]);

    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:m1:8V:CORR:GAIN:NODE:0", "0.998003;6.399530"), ZSCPI::scpiAnswer[ZSCPI::ack]);
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:m1:8V:CORR:GAIN:NODE:1", "0.998573;9.599050"), ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_adj_procedure::setPhaseNodesOnTwoRanges()
{
    // taken from mt310s2-050059467.xml
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:PHASE:NODE:0", "-0.000588;42.998000"), ZSCPI::scpiAnswer[ZSCPI::ack]);
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:m1:250V:CORR:PHASE:NODE:1", "-0.001490;62.996000"), ZSCPI::scpiAnswer[ZSCPI::ack]);

    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:m2:8V:CORR:PHASE:NODE:0", "0.010366;43.000000"), ZSCPI::scpiAnswer[ZSCPI::ack]);
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:m2:8V:CORR:PHASE:NODE:1", "-0.006581;62.996000"), ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_adj_procedure::writeAdjToChip()
{
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", ""), ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_adj_procedure::computeAdj()
{
    QCOMPARE(ScpiSingleTransactionBlocked::cmd("SENS:CORR:COMP", ""), ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_adj_procedure::simulReboot()
{
    qInfo("Simul reboot...");
    cleanup();
    setupServers();
}
