#include "test_regression_sense_interface_com5003.h"
#include "testfactoryi2cctrl.h"
#include "senseregressionhelper.h"
#include "scpisingletransactionblocked.h"
#include "proxy.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <testloghelpers.h>
#include <QRegularExpression>
#include <QJsonValue>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_sense_interface_com5003);

void test_regression_sense_interface_com5003::init()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(
        std::make_shared<TestFactoryI2cCtrl>(true),
        tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_sense_interface_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_sense_interface_com5003::checkVersionsOfSystemInterface()
{
    QCOMPARE(m_testServer->getDeviceVersion(), "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
}

QStringList test_regression_sense_interface_com5003::m_channelsExpectedAllOverThePlace = QStringList()
                                                                                         << "m0" << "m1" << "m2" << "m3" << "m4" << "m5";

void test_regression_sense_interface_com5003::checkChannelCatalogAsExpected()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getChannelList();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy[0][2].toStringList(), m_channelsExpectedAllOverThePlace);
}

// Is that a good idea to put it on modules's to hide reference ranges???
QStringList test_regression_sense_interface_com5003::m_rangesExpectedU = QStringList()
                                                                         << "480V" << "240V" << "120V" << "60V" << "12V" << "5V"
                                                                         << "R0V" << "R10V";

void test_regression_sense_interface_com5003::checkRangesUL1()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();
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
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedI);
}

void test_regression_sense_interface_com5003::constantRangeValuesI()
{
    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3"));
    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/all-ranges-i.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_com5003::constantRangeValuesU()
{
    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3"));
    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/all-ranges-u.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_com5003::constantRangeValuesIModeREF()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "REF");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3"));
    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/all-ranges-i-ref-mode.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_com5003::constantRangeValuesUModeREF()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "REF");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3"));
    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/all-ranges-u-ref-mode.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_com5003::channelAliasChangeOnREF()
{
    QString channelM0AliasBefore = ScpiSingleTransactionBlocked::query("SENSE:m0:ALIAS?");
    QCOMPARE(channelM0AliasBefore, "UL1");

    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "REF");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString channelM0AliasAfter = ScpiSingleTransactionBlocked::query("SENSE:m0:ALIAS?");
    QCOMPARE(channelM0AliasAfter, "REF1");
}

void test_regression_sense_interface_com5003::mmodeCat()
{
    // original COM implementation was sorted
    QString mmodeCat = ScpiSingleTransactionBlocked::query("SENSE:MMODE:CAT?");
    QCOMPARE(mmodeCat, "AC;REF");
}

void test_regression_sense_interface_com5003::invalidMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "foo");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_interface_com5003::twiceSameMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "AC");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "AC");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

QJsonObject test_regression_sense_interface_com5003::loadJson(QString fileName)
{
    QFile referencFile(fileName);
    referencFile.open(QFile::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(referencFile.readAll());
    return doc.object();
}
