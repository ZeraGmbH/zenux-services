#include "test_regression_sense_interface_mt581s2.h"
#include "clamp.h"
#include "testfactoryi2cctrl.h"
#include "senseregressionhelper.h"
#include "scpisingletransactionblocked.h"
#include "proxy.h"
#include "zscpi_response_definitions.h"
#include <i2cmultiplexerfactory.h>
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <testloghelpers.h>
#include <zera-jsonfileloader.h>
#include <QFile>
#include <QRegularExpression>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_sense_interface_mt581s2);

void test_regression_sense_interface_mt581s2::initTestCase()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(
        std::make_shared<TestFactoryI2cCtrl>(true),
        tcpNetworkFactory,
        "mt581s2");
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_sense_interface_mt581s2::init()
{
    ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "AC");
}

void test_regression_sense_interface_mt581s2::checkVersionsOfSystemInterface()
{
    QCOMPARE(m_testServer->getDeviceVersion(), "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
}

QStringList test_regression_sense_interface_mt581s2::m_channelsExpectedAllOverThePlace = QStringList()
                                                                                               << "m0" << "m1" << "m2" << "m6" << "m3" << "m4" << "m5" << "m7";


void test_regression_sense_interface_mt581s2::checkChannelCatalogAsExpected()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getChannelList();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy[0][2].toStringList(), m_channelsExpectedAllOverThePlace);
}

QStringList test_regression_sense_interface_mt581s2::m_rangesExpectedU = QStringList()
    << "480V" << "250V" << "125V" << "60V" << "8V" << "100mV";

void test_regression_sense_interface_mt581s2::checkRangesUL1()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedU);
}

// Is that a good idea to put it on modules's to hide internal current (voltage) ranges???
QStringList test_regression_sense_interface_mt581s2::m_rangesExpectedI = QStringList()
                                                                         << "100A" << "50A" << "25A" << "10A" << "5A" << "2.5A" << "1.0A"
                                                                         << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
QStringList test_regression_sense_interface_mt581s2::m_rangesExpectedI_Internal = QStringList()
                                                                         << "8V" << "5V" << "2V" << "1V"
                                                                         << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";

void test_regression_sense_interface_mt581s2::checkRangesIL1()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal);
}

QStringList test_regression_sense_interface_mt581s2::m_rangesExpectedI_CL120A = QStringList()
                                                                                << "C100A" << "C50A" << "C10A" << "C5A" << "C1A"
                                                                                << "C500mA" << "C100mA" << "C50mA" << "C10mA";


void test_regression_sense_interface_mt581s2::addClampIL1_CL120A()
{
    SenseSystem::cChannelSettings *channelSettingI = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1");
    SenseSystem::cChannelSettings *channelSettingU = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1");

    m_testServer->addClamp(cClamp::CL120A, "IL1");

    QSignalSpy responseSpyI(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal + m_rangesExpectedI_CL120A);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU);
}

QStringList test_regression_sense_interface_mt581s2::m_rangesExpectedU_CL800ADC1000VDC = QStringList() << "C1000V";
QStringList test_regression_sense_interface_mt581s2::m_rangesExpectedI_CL800ADC1000VDC = QStringList()
                                                                                         << "C800A" << "C400A" << "C200A" << "C80A" << "C40A" << "C20A";


void test_regression_sense_interface_mt581s2::addClampIL2_CL800ADC1000VDC()
{
    SenseSystem::cChannelSettings *channelSettingI = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL2");
    SenseSystem::cChannelSettings *channelSettingU = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL2");

    m_testServer->addClamp(cClamp::CL800ADC1000VDC, "IL2");

    QSignalSpy responseSpyI(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal + m_rangesExpectedI_CL800ADC1000VDC);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU + m_rangesExpectedU_CL800ADC1000VDC);
}


QStringList test_regression_sense_interface_mt581s2::m_rangesExpectedI_DummyAux = QStringList() << "0A";

void test_regression_sense_interface_mt581s2::addRemoveClampIAUX_CL800ADC1000VDC()
{
    SenseSystem::cChannelSettings *channelSettingI = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    SenseSystem::cChannelSettings *channelSettingU = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UAUX");

    m_testServer->addClamp(cClamp::CL800ADC1000VDC, "IAUX");

    QSignalSpy responseSpyIWishClamps(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyIWishClamps[0][2].toStringList(), m_rangesExpectedI_DummyAux + m_rangesExpectedI_Internal + m_rangesExpectedI_CL800ADC1000VDC);

    QSignalSpy responseSpyU1(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU1[0][2].toStringList(), m_rangesExpectedU + m_rangesExpectedU_CL800ADC1000VDC);

    m_testServer->removeAllClamps();

    QSignalSpy responseSpyI(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI_DummyAux + m_rangesExpectedI_Internal);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU);
}

void test_regression_sense_interface_mt581s2::clampIdsNamesGenJson()
{
    QJsonObject jsonAll;
    for(int clampTypeNo=cClamp::undefined+1; clampTypeNo<cClamp::anzCL; clampTypeNo++) {
        QString clampName = cClamp::getClampTypeName(clampTypeNo);
        jsonAll.insert(SenseRegressionHelper::getJsonNumString(clampTypeNo), clampName);
    }
    QJsonDocument doc(jsonAll);
    qInfo("----------------- json id / names generated -----------------");
    qInfo("%s", qPrintable(doc.toJson(QJsonDocument::Indented)));
}

void test_regression_sense_interface_mt581s2::clampIdsNamesCheck()
{
    QJsonObject json = cJsonFileLoader::loadJsonFile(":/clamp-id-names.json");
    QVERIFY(!json.isEmpty());
    int countClamps = cClamp::anzCL-1; // anzCL lies!!!
    QCOMPARE(json.count(), countClamps);
    for(int clampTypeNo=cClamp::undefined+1; clampTypeNo<cClamp::anzCL; clampTypeNo++) {
        QString jsonKey = SenseRegressionHelper::getJsonNumString(clampTypeNo);
        QCOMPARE(cClamp::getClampTypeName(clampTypeNo), json.value(jsonKey).toString());
    }
}

void test_regression_sense_interface_mt581s2::constantRangeValuesI()
{
    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX"));

    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-i.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::constantRangeValuesU()
{
    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UAUX"));

    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-u.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::constantRangeValuesIModeAdj()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "ADJ");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX"));

    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-i-adj-mode.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::constantRangeValuesUModeAdj()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "ADJ");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UAUX"));

    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-u-adj-mode.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::constantRangeValuesIModeHf()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "HF");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX"));

    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-i-hf-mode.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::constantRangeValuesUModeHf()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "HF");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL2"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3"));
    channelSettings.append(m_testServer->getSenseSettings()->findChannelSettingByAlias1("UAUX"));

    QByteArray jsonDumped = SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSettings, m_pcbIFace.get());
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-u-hf-mode.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::constantRangeValuesAllClampsIL3()
{
    QByteArray jsonDumped = genJsonConstantValuesAllRangesForAllClamps("IL3", "IL3");
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-all-clamps-il3.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::constantRangeValuesAllClampsIAUX()
{
    QByteArray jsonDumped = genJsonConstantValuesAllRangesForAllClamps("IAUX", "IAUX");
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-all-clamps-iaux.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::constantRangeValuesAllClampsUAUX()
{
    QByteArray jsonDumped = genJsonConstantValuesAllRangesForAllClamps("UAUX", "IAUX");
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/mt581s2/all-ranges-all-clamps-uaux.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_regression_sense_interface_mt581s2::mmodeCat()
{
    // original MT implementation was hash random
    QString mmodeCat = ScpiSingleTransactionBlocked::query("SENSE:MMODE:CAT?");
    QStringList mmodes = mmodeCat.split(";");
    QCOMPARE(mmodes.count(), 3);
    QVERIFY(mmodes.contains("AC"));
    QVERIFY(mmodes.contains("ADJ"));
    QVERIFY(mmodes.contains("HF"));
}

void test_regression_sense_interface_mt581s2::invalidMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "foo");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_interface_mt581s2::twiceSameMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "AC");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "AC");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_sense_interface_mt581s2::channelAliasAcMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "AC");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString channelM0AliasAfter = ScpiSingleTransactionBlocked::query("SENSE:m0:ALIAS?");
    QCOMPARE(channelM0AliasAfter, "UL1");
}

void test_regression_sense_interface_mt581s2::channelAliasAdjMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "ADJ");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString channelM0AliasAfter = ScpiSingleTransactionBlocked::query("SENSE:m0:ALIAS?");
    QCOMPARE(channelM0AliasAfter, "UL1");
}

void test_regression_sense_interface_mt581s2::channelAliasHfMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "HF");
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString channelM0AliasAfter = ScpiSingleTransactionBlocked::query("SENSE:m0:ALIAS?");
    QCOMPARE(channelM0AliasAfter, "UL1");
}


QByteArray test_regression_sense_interface_mt581s2::genJsonConstantValuesAllRangesForAllClamps(QString channelName, QString channelNameAdRemoveClamps)
{
    if(channelNameAdRemoveClamps.isEmpty())
        channelNameAdRemoveClamps = channelName;
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1(channelName);
    QJsonObject jsonAll;
    for(int clampTypeNo=cClamp::undefined+1; clampTypeNo<cClamp::anzCL; clampTypeNo++) { // all clamp types
        m_testServer->addClamp(clampTypeNo, channelNameAdRemoveClamps);

        QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
        m_pcbIFace->getRangeList(channelSetting->m_nameMx);
        TimeMachineObject::feedEventLoop();

        QJsonArray jsonRanges;
        const QStringList ranges = responseSpy[0][2].toStringList();
        for(const QString &range : ranges) {
            QJsonObject jsonRange;
            SenseRegressionHelper::addRangeConstantDataToJson(range, channelSetting, jsonRange);
            jsonRanges.append(jsonRange);
        }
        m_testServer->removeAllClamps();

        jsonAll.insert(cClamp::getClampTypeName(clampTypeNo), jsonRanges);
    }
    QJsonDocument doc(jsonAll);
    return doc.toJson(QJsonDocument::Indented);
}
