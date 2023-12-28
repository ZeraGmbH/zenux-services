#include "test_regression_sense_interface_com5003.h"
#include "proxy.h"
#include "pcbinterface.h"
#include <timemachineobject.h>
#include "regressionhelper.h"
#include <QRegularExpression>
#include <QJsonValue>
#include <QJsonDocument>
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

void test_regression_sense_interface_com5003::checkImportXmlMinimal()
{
    QString filenameShort = ":/regression_data/adjustment_export_minimal_pass";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_sense_interface_com5003::checkImportXmlFull()
{
    QString filenameShort = ":/regression_data/adjustment_export";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_sense_interface_com5003::checkImportMissingType()
{
    QString filenameShort = ":/regression_data/adjustment_export_missing_type";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    // Currently there is no type check -> pass
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_sense_interface_com5003::checkImportMissingSerNo()
{
    QString filenameShort = ":/regression_data/adjustment_export_missing_serno";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
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
    SenseSystem::cChannelSettings *channelSetting = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("UL1");

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
    SenseSystem::cChannelSettings *channelSetting = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("IL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedI);
}

void test_regression_sense_interface_com5003::constantRangeValuesIL3GenJson()
{
    genJsonConstantValuesAllRanges("IL3");
}

void test_regression_sense_interface_com5003::constantRangeValuesIL3Check()
{
    QJsonObject json = loadJson(":/regression_data/all-ranges-il3.json");
    QVERIFY(!json.isEmpty());
    QVERIFY(checkJsonConstantValuesAllRanges(json, "IL3"));

}

static QString noClampJsonId = QStringLiteral("no-clamps");

void test_regression_sense_interface_com5003::genJsonConstantValuesAllRanges(QString channelName)
{
    SenseSystem::cChannelSettings *channelSetting = m_mockServer->getSenseSettings()->findChannelSettingByAlias1(channelName);
    QJsonObject jsonAll;

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QJsonObject jsonRanges;
    const QStringList ranges = responseSpy[0][2].toStringList();
    for(const QString &range : ranges) {
        QJsonObject jsonRange;
        RegressionHelper::addRangeConstantDataToJson(range, channelSetting, jsonRange);
        jsonRanges.insert(range, jsonRange);
    }

    // same struture ad mt310s2 - no clamps yet
    jsonAll.insert(noClampJsonId, jsonRanges);

    QJsonDocument doc(jsonAll);
    qInfo("----------------- json range constants generated for %s -----------------", qPrintable(channelName));
    qInfo("%s", qPrintable(doc.toJson(QJsonDocument::Indented)));
}

bool test_regression_sense_interface_com5003::checkJsonConstantValuesAllRanges(QJsonObject jsonReference, QString channelName)
{
    bool allCheckOk = true;
    SenseSystem::cChannelSettings *channelSetting = m_mockServer->getSenseSettings()->findChannelSettingByAlias1(channelName);

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    if(jsonReference.contains(noClampJsonId)) {
        QJsonObject jsonRanges = jsonReference.value(noClampJsonId).toObject();
        if(!jsonRanges.isEmpty()) {
            const QStringList ranges = responseSpy[0][2].toStringList();
            if(!ranges.isEmpty()) {
                for(const QString &range : ranges) {
                    QJsonObject jsonRange = jsonRanges.value(range).toObject();
                    if(!RegressionHelper::compareRangeConstantDataWithJson(jsonRange, noClampJsonId, range, channelSetting))
                        allCheckOk = false;
                }
            }
            else {
                allCheckOk = false;
                qCritical("No ranges returned from device for clamp \"%s\"", qPrintable(noClampJsonId));
            }
        }
        else {
            allCheckOk = false;
            qCritical("No ranges found in reference for clamp \"%s\"", qPrintable(noClampJsonId));
        }
    }
    else {
        allCheckOk = false;
        qCritical("Clamp \"%s\" not found in reference", qPrintable(noClampJsonId));
    }
    return allCheckOk;
}

QJsonObject test_regression_sense_interface_com5003::loadJson(QString fileName)
{
    QFile referencFile(fileName);
    referencFile.open(QFile::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(referencFile.readAll());
    return doc.object();
}
