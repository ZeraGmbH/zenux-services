#include "test_regression_sense_interface_com5003.h"
#include "proxy.h"
#include "reply.h"
#include "pcbinterface.h"
#include <timemachineobject.h>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_sense_interface_com5003);

void test_regression_sense_interface_com5003::init()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceCom5003>();
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_sense_interface_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_sense_interface_com5003::checkVersionsOfSystemInterface()
{
    QCOMPARE(m_mockServer->getDeviceVersion(), "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
}

void test_regression_sense_interface_com5003::checkExportXml()
{
    // Date time are empty. On change: see mt310s2 and add XmlHelperForTest::removeTimeDependentEntriesFromXml
    QString xmlExported = m_mockServer->getAdjustment()->exportXMLString();
    qInfo("Exported XML:");
    qInfo("%s", qPrintable(xmlExported));

    QFile xmlFile(":/regression_data/adjustment_export.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    qInfo("Expected XML:");
    qInfo("%s", qPrintable(xmlExpected));

    // if this turns fragile we have to use zera-scpi's xml-compare-testlib
    QCOMPARE(xmlExported, xmlExpected);
}

QStringList test_regression_sense_interface_com5003::m_channelsExpectedAllOverThePlace = QStringList()
                                                                                         << "m0" << "m1" << "m2" << "m3" << "m4" << "m5";

void test_regression_sense_interface_com5003::checkChannelCatalogAsExpected()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getChannelList();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2].toStringList(), m_channelsExpectedAllOverThePlace);
}

// Is that a good idea to put it on modules's to hide reference ranges???
QStringList test_regression_sense_interface_com5003::m_rangesExpectedU = QStringList()
                                                                         << "480V" << "240V" << "120V" << "60V" << "12V" << "5V"
                                                                         << "R0V" << "R10V";

void test_regression_sense_interface_com5003::checkRangesUL1()
{
    SenseSystem::cChannelSettings *channelSetting = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("UL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedU);
}

// Is that a good idea to put it on modules's to hide reference ranges???
QStringList test_regression_sense_interface_com5003::m_rangesExpectedI = QStringList()
                                                                         << "200A" << "100A" << "50A" << "25A" << "10A"
                                                                         << "5A" << "2.5A" << "1.0A"
                                                                         << "500mA" << "250mA" << "100mA" << "50mA" << "25mA" << "10mA" << "5mA"
                                                                         << "R0V" << "R10V";

void test_regression_sense_interface_com5003::checkRangesIL1()
{
    SenseSystem::cChannelSettings *channelSetting = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("IL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedI);
}
