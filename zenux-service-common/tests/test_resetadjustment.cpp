#include "test_resetadjustment.h"
#include "mockserverparamgenerator.h"
#include "mockeepromdevice.h"
#include "adjflags.h"
#include "zscpi_response_definitions.h"
#include "scpisingletransactionblocked.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "mt310s2systeminfomock.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_resetAdjustment)

void test_resetAdjustment::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);

    Mt310s2SystemInfoMock::setDeviceName("MT310s2 ADW5859");
    Mt310s2SystemInfoMock::setSerialNumber("050059467");
}

void test_resetAdjustment::init()
{
    MockEepromDevice::cleanAll();
}

void test_resetAdjustment::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_resetAdjustment::resetAdjDataOk()
{
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams("mt310s2d"));
    I2cSettingsPtr i2cSettings = settings->getI2cSettings();
    MockEepromDevice::setData({i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
                              TestLogHelpers::loadFile(":/mt310s2-050059467.eeprom"));
    setupServers();

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::adjusted));

    ret = ScpiSingleTransactionBlocked::cmd("SENSE:RESET:ADJUSTMENT", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::notAdjusted));
}

void test_resetAdjustment::setupServers()
{
    AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true);
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);

    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(
        ctrlFactory,
        m_tcpNetworkFactory,
        "mt310s2",
        true);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
