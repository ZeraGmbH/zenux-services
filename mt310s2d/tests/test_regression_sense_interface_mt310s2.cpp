#include "test_regression_sense_interface_mt310s2.h"
#include "testfactoryi2cctrl.h"
#include "senseregressionhelper.h"
#include "scpisingletransactionblocked.h"
#include "proxy.h"
#include "pcbinterface.h"
#include "clampfactorytest.h"
#include <i2cmultiplexerfactory.h>
#include <timemachineobject.h>
#include <QFile>
#include <QRegularExpression>
#include <QJsonValue>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_sense_interface_mt310s2);

void test_regression_sense_interface_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true));
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_sense_interface_mt310s2::init()
{
    ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "AC");
}

void test_regression_sense_interface_mt310s2::checkVersionsOfSystemInterface()
{
    QCOMPARE(m_testServer->getDeviceVersion(), "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
}

QStringList test_regression_sense_interface_mt310s2::m_channelsExpectedAllOverThePlace = QStringList()
                                                                                               << "m0" << "m1" << "m2" << "m6" << "m3" << "m4" << "m5" << "m7";


void test_regression_sense_interface_mt310s2::checkChannelCatalogAsExpected()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getChannelList();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy[0][2].toStringList(), m_channelsExpectedAllOverThePlace);
}

QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedU = QStringList()
    << "250V" << "8V" << "100mV";

void test_regression_sense_interface_mt310s2::checkRangesUL1()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedU);
}

// Is that a good idea to put it on modules's to hide internal current (voltage) ranges???
QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI = QStringList()
                                                                         << "10A" << "5A" << "2.5A" << "1.0A"
                                                                         << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI_Internal = QStringList()
                                                                         << "8V" << "5V" << "2V" << "1V"
                                                                         << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";

void test_regression_sense_interface_mt310s2::checkRangesIL1()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal);
}

QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI_CL120A = QStringList()
                                                                                << "C100A" << "C50A" << "C10A" << "C5A" << "C1A"
                                                                                << "C500mA" << "C100mA" << "C50mA" << "C10mA";


void test_regression_sense_interface_mt310s2::addClampIL1_CL120A()
{
    SenseSystem::cChannelSettings *channelSettingI = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL1");
    SenseSystem::cChannelSettings *channelSettingU = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL1");

    m_testServer->addClamp(CL120A, "IL1");

    QSignalSpy responseSpyI(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal + m_rangesExpectedI_CL120A);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU);
}

QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedU_CL800ADC1000VDC = QStringList() << "C1000V";
QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI_CL800ADC1000VDC = QStringList()
                                                                                         << "C800A" << "C400A" << "C200A" << "C80A" << "C40A" << "C20A";


void test_regression_sense_interface_mt310s2::addClampIL2_CL800ADC1000VDC()
{
    SenseSystem::cChannelSettings *channelSettingI = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL2");
    SenseSystem::cChannelSettings *channelSettingU = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL2");

    m_testServer->addClamp(CL800ADC1000VDC, "IL2");

    QSignalSpy responseSpyI(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal + m_rangesExpectedI_CL800ADC1000VDC);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU + m_rangesExpectedU_CL800ADC1000VDC);
}


QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI_DummyAux = QStringList() << "0A";

void test_regression_sense_interface_mt310s2::addRemoveClampIAUX_CL800ADC1000VDC()
{
    SenseSystem::cChannelSettings *channelSettingI = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    SenseSystem::cChannelSettings *channelSettingU = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UAUX");

    m_testServer->addClamp(CL800ADC1000VDC, "IAUX");

    QSignalSpy responseSpyIWishClamps(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyIWishClamps[0][2].toStringList(), m_rangesExpectedI_DummyAux + m_rangesExpectedI_Internal + m_rangesExpectedI_CL800ADC1000VDC);

    QSignalSpy responseSpyU1(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU1[0][2].toStringList(), m_rangesExpectedU + m_rangesExpectedU_CL800ADC1000VDC);

    m_testServer->removeAllClamps();

    QSignalSpy responseSpyI(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI_DummyAux + m_rangesExpectedI_Internal);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU);
}

void test_regression_sense_interface_mt310s2::clampIdsNamesGenJson()
{
    QJsonObject jsonAll;
    for(int clampTypeNo=undefined+1; clampTypeNo<anzCL; clampTypeNo++) {
        QString clampName = cClamp::getClampTypeName(clampTypeNo);
        jsonAll.insert(SenseRegressionHelper::getJsonNumString(clampTypeNo), clampName);
    }
    QJsonDocument doc(jsonAll);
    qInfo("----------------- json id / names generated -----------------");
    qInfo("%s", qPrintable(doc.toJson(QJsonDocument::Indented)));
}

void test_regression_sense_interface_mt310s2::clampIdsNamesCheck()
{
    QJsonObject json = loadJson(":/clamp-id-names.json");
    QVERIFY(!json.isEmpty());
    int countClamps = anzCL-1; // anzCL lies!!!
    QCOMPARE(json.count(), countClamps);
    for(int clampTypeNo=undefined+1; clampTypeNo<anzCL; clampTypeNo++) {
        QString jsonKey = SenseRegressionHelper::getJsonNumString(clampTypeNo);
        QCOMPARE(cClamp::getClampTypeName(clampTypeNo), json.value(jsonKey).toString());
    }
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIL3GenJson()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIL3Check()
{
    QJsonObject json = loadJson(":/all-ranges-il3.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesUL3GenJson()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_mt310s2::constantRangeValuesUL3Check()
{
    QJsonObject json = loadJson(":/all-ranges-ul3.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UL3");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIAUXGenJson()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIAUXCheck()
{
    QJsonObject json = loadJson(":/all-ranges-iaux.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesUAUXGenJson()
{
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_mt310s2::constantRangeValuesUAUXCheck()
{
    QJsonObject json = loadJson(":/all-ranges-uaux.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("UAUX");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIL3ModeAdjGenJson()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "ADJ");
    QCOMPARE(answer, "ack");

    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIL3ModeAdjCheck()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "ADJ");
    QCOMPARE(answer, "ack");

    QJsonObject json = loadJson(":/all-ranges-il3-adj-mode.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIAUXModeAdjGenJson()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "ADJ");
    QCOMPARE(answer, "ack");

    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIAUXModeAdjCheck()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "ADJ");
    QCOMPARE(answer, "ack");

    QJsonObject json = loadJson(":/all-ranges-iaux-adj-mode.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIL3ModeHfGenJson()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "HF");
    QCOMPARE(answer, "ack");

    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIL3ModeHfCheck()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "HF");
    QCOMPARE(answer, "ack");

    QJsonObject json = loadJson(":/all-ranges-il3-hf-mode.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IL3");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIAUXModeHfGenJson()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "HF");
    QCOMPARE(answer, "ack");

    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    SenseRegressionHelper::genJsonConstantValuesAllRanges(channelSetting, m_pcbIFace.get());
}

void test_regression_sense_interface_mt310s2::constantRangeValuesIAUXModeHfCheck()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "HF");
    QCOMPARE(answer, "ack");

    QJsonObject json = loadJson(":/all-ranges-iaux-hf-mode.json");
    QVERIFY(!json.isEmpty());
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    QVERIFY(SenseRegressionHelper::checkJsonConstantValuesAllRanges(json, channelSetting, m_pcbIFace.get()));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesAllClampsIL3GenJson()
{
    genJsonConstantValuesAllRangesForAllClamps("IL3", "IL3");
}

void test_regression_sense_interface_mt310s2::constantRangeValuesAllClampsIL3Check()
{
    QJsonObject json = loadJson(":/all-ranges-all-clamps-il3.json");
    QVERIFY(!json.isEmpty());
    QVERIFY(checkJsonConstantValuesAllRangesForAllClamps(json, "IL3", "IL3"));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesAllClampsIAUXGenJson()
{
    genJsonConstantValuesAllRangesForAllClamps("IAUX", "IAUX");
}

void test_regression_sense_interface_mt310s2::constantRangeValuesAllClampsIAUXCheck()
{
    QJsonObject json = loadJson(":/all-ranges-all-clamps-iaux.json");
    QVERIFY(!json.isEmpty());
    QVERIFY(checkJsonConstantValuesAllRangesForAllClamps(json, "IAUX", "IAUX"));
}

void test_regression_sense_interface_mt310s2::constantRangeValuesAllClampsUAUXGenJson()
{
    genJsonConstantValuesAllRangesForAllClamps("UAUX", "IAUX");
}

void test_regression_sense_interface_mt310s2::constantRangeValuesAllClampsUAUXCheck()
{
    QJsonObject json = loadJson(":/all-ranges-all-clamps-uaux.json");
    QVERIFY(!json.isEmpty());
    QVERIFY(checkJsonConstantValuesAllRangesForAllClamps(json, "UAUX", "IAUX"));
}

void test_regression_sense_interface_mt310s2::channelAliasAcMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "AC");
    QCOMPARE(answer, "ack");

    QString channelM0AliasAfter = ScpiSingleTransactionBlocked::query("SENSE:m0:ALIAS?");
    QCOMPARE(channelM0AliasAfter, "UL1");
}

void test_regression_sense_interface_mt310s2::channelAliasAdjMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "ADJ");
    QCOMPARE(answer, "ack");

    QString channelM0AliasAfter = ScpiSingleTransactionBlocked::query("SENSE:m0:ALIAS?");
    QCOMPARE(channelM0AliasAfter, "UL1");
}

void test_regression_sense_interface_mt310s2::channelAliasHfMode()
{
    QString answer = ScpiSingleTransactionBlocked::cmd("SENS:MMODE", "HF");
    QCOMPARE(answer, "ack");

    QString channelM0AliasAfter = ScpiSingleTransactionBlocked::query("SENSE:m0:ALIAS?");
    QCOMPARE(channelM0AliasAfter, "UL1");
}


void test_regression_sense_interface_mt310s2::genJsonConstantValuesAllRangesForAllClamps(QString channelName, QString channelNameAdRemoveClamps)
{
    if(channelNameAdRemoveClamps.isEmpty())
        channelNameAdRemoveClamps = channelName;
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1(channelName);
    QJsonObject jsonAll;
    for(int clampTypeNo=undefined+1; clampTypeNo<anzCL; clampTypeNo++) { // all clamp types
        m_testServer->addClamp(clampTypeNo, channelNameAdRemoveClamps);

        QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
        m_pcbIFace->getRangeList(channelSetting->m_nameMx);
        TimeMachineObject::feedEventLoop();

        QJsonObject jsonRanges;
        const QStringList ranges = responseSpy[0][2].toStringList();
        for(const QString &range : ranges) {
            QJsonObject jsonRange;
            SenseRegressionHelper::addRangeConstantDataToJson(range, channelSetting, jsonRange);
            jsonRanges.insert(range, jsonRange);
        }
        m_testServer->removeAllClamps();

        jsonAll.insert(cClamp::getClampTypeName(clampTypeNo), jsonRanges);
    }
    QJsonDocument doc(jsonAll);
    qInfo("----------------- json range constants generated for %s -----------------", qPrintable(channelName));
    qInfo("%s", qPrintable(doc.toJson(QJsonDocument::Indented)));
}

bool test_regression_sense_interface_mt310s2::checkJsonConstantValuesAllRangesForAllClamps(QJsonObject jsonReference, QString channelName, QString channelNameAdRemoveClamps)
{
    if(channelNameAdRemoveClamps.isEmpty())
        channelNameAdRemoveClamps = channelName;
    bool allCheckOk = true;
    SenseSystem::cChannelSettings *channelSetting = m_testServer->getSenseSettings()->findChannelSettingByAlias1(channelName);
    for(int clampTypeNo=undefined+1; clampTypeNo<anzCL; clampTypeNo++) { // all clamp types
        m_testServer->addClamp(clampTypeNo, channelNameAdRemoveClamps);

        QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
        m_pcbIFace->getRangeList(channelSetting->m_nameMx);
        TimeMachineObject::feedEventLoop();

        QString clampName = cClamp::getClampTypeName(clampTypeNo);
        if(jsonReference.contains(clampName)) {
            QJsonObject jsonRanges = jsonReference.value(clampName).toObject();
            if(!jsonRanges.isEmpty()) {
                const QStringList ranges = responseSpy[0][2].toStringList();
                if(!ranges.isEmpty()) {
                    for(const QString &range : ranges) {
                        QJsonObject jsonRange = jsonRanges.value(range).toObject();
                        if(!SenseRegressionHelper::compareRangeConstantDataWithJson(jsonRange, clampName, range, channelSetting))
                            allCheckOk = false;
                    }
                }
                else {
                    allCheckOk = false;
                    qCritical("No ranges returned from device for clamp \"%s\"", qPrintable(clampName));
                }
            }
            else {
                allCheckOk = false;
                qCritical("No ranges found in reference for clamp \"%s\"", qPrintable(clampName));
            }
        }
        else {
            allCheckOk = false;
            qCritical("Clamp \"%s\" not found in reference", qPrintable(clampName));
        }
        m_testServer->removeAllClamps();
    }
    return allCheckOk;
}

QJsonObject test_regression_sense_interface_mt310s2::loadJson(QString fileName)
{
    QFile referencFile(fileName);
    referencFile.open(QFile::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(referencFile.readAll());
    return doc.object();
}
