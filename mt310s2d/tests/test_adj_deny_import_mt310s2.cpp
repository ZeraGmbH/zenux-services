#include "test_adj_deny_import_mt310s2.h"
#include "testfactoryi2cctrl.h"
#include "mt310s2systeminfomock.h"
#include "proxy.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"
#include "mockeepromdevice.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_adj_deny_import_mt310s2);

void test_adj_deny_import_mt310s2::init()
{
    MockEepromDevice::mockCleanAll();
    MockI2cEEpromIoFactory::enableMock();
}

void test_adj_deny_import_mt310s2::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_deny_import_mt310s2::loadEEpromWithStoredNamesAndVersions()
{
    std::unique_ptr<SettingsContainer> settings =  std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("mt310s2d"));
    I2cSettings *i2cSettings = settings->getI2cSettings();
    QVERIFY(MockEepromDevice::mockReadFromFile({ i2cSettings->getDeviceNode(),
                                                 i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress) },
                                               ":/export_internal_modified.eeprom"));
    setupServers();

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));

    QFile xmlFile(":/import_modified.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = XmlHelperForTest::prepareForCompare(xmlFile.readAll());

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_adj_deny_import_mt310s2::loadEEpromAndDenyDifferentDeviceName()
{
    std::unique_ptr<SettingsContainer> settings =  std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("mt310s2d"));
    I2cSettings *i2cSettings = settings->getI2cSettings();
    QVERIFY(MockEepromDevice::mockReadFromFile({ i2cSettings->getDeviceNode(),
                                                 i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress) },
                                               ":/export_internal_modified.eeprom"));
    setupServers();

    static_cast<Mt310s2SystemInfoMock*>(m_testServer->getSystemInfo())->setDeviceName("Foo");

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_deny_import_mt310s2::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(
        std::make_shared<TestFactoryI2cCtrl>(true),
        tcpNetworkFactory,
        "mt310s2",
        true);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
