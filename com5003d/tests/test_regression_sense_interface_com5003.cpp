#include "test_regression_sense_interface_com5003.h"
#include "testfactoryi2cctrl.h"
#include "senseregressionhelper.h"
#include "scpisingletransactionblocked.h"
#include "proxy.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <QRegularExpression>
#include <QJsonValue>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_sense_interface_com5003);

void test_regression_sense_interface_com5003::init()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(std::make_shared<TestFactoryI2cCtrl>(true));
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

void test_regression_sense_interface_com5003::constantRangeValuesIL3GenJson()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_com5003::constantRangeValuesIL3Check()
{
    QJsonObject json = loadJson(":/all-ranges-il3.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_com5003::constantRangeValuesUL3GenJson()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_com5003::constantRangeValuesUL3Check()
{
    QJsonObject json = loadJson(":/all-ranges-ul3.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
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
    QString mmodeCat = ScpiSingleTransactionBlocked::query("SENSE:MMODE:CAT?");
    QStringList mmodes = mmodeCat.split(";");
    QCOMPARE(mmodes.count(), 2);
    QVERIFY(mmodes.contains("AC"));
    QVERIFY(mmodes.contains("REF"));
}

QJsonObject test_regression_sense_interface_com5003::loadJson(QString fileName)
{
    QFile referencFile(fileName);
    referencFile.open(QFile::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(referencFile.readAll());
    return doc.object();
}
