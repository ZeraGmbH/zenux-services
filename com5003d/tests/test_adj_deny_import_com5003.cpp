#include "test_adj_deny_import_com5003.h"
#include "testfactoryi2cctrl.h"
#include "testsysteminfo.h"
#include "proxy.h"
#include "mockserverparamgenerator.h"
#include "mockeepromdevice.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timerfactoryqtfortest.h>
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_adj_deny_import_com5003);

void test_adj_deny_import_com5003::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_adj_deny_import_com5003::cleanup()
{
    MockEepromDevice::cleanAll();
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_adj_deny_import_com5003::loadEEpromWithStoredNamesAndVersions()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    I2cSettingsPtr i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/export_internal_modified.eeprom"));
    setupServers();

    QString xmlExported = XmlHelperForTest::prepareForCompare(ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?"));
    QString xmlExpected = TestLogHelpers::loadFile(":/import_modified.xml");
    xmlExpected = XmlHelperForTest::prepareForCompare(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_adj_deny_import_com5003::loadEEpromAndDenyDifferentDeviceName()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("com5003d"));
    const I2cSettingsPtr i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/export_internal_modified.eeprom"));
    setupServers();

    static_cast<TestSystemInfo*>(m_testServer->getSystemInfo())->setDeviceName("Foo");

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_adj_deny_import_com5003::setupServers()
{
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(
        std::make_shared<TestFactoryI2cCtrl>(true),
        m_tcpNetworkFactory,
        "com5003",
        true);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
