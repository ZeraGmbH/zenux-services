#include "test_adj_data_serialization.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "mockeeprom24lc.h"
#include "mocki2ceepromiofactory.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QFile>
#include <QTest>

QTEST_MAIN(test_adj_data_serialization)

void test_adj_data_serialization::FromAdjToXMLFileForMt310s2()
{
    MockEEprom24LC::mockCleanAll();
    MockI2cEEpromIoFactory::enableMock();
    setupServers("MT310s2 ADW5859", "050059467");

    I2cSettings *i2cSettings = m_testServer->getI2cSettings();
    QVERIFY(MockEEprom24LC::mockReadFromFile(i2cSettings->getDeviceNode(),
                                             i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress),
                                             ":/mt310s2-050059467.eeprom"));

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

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

void test_adj_data_serialization::setupServers(QString deviceName, QString serialNum)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true));
    m_testServer->setDeviceName(deviceName);
    m_testServer->setSerialNum(serialNum);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();

}


