#include "test_regression_sense_interface_mt310s2.h"
#include "clampinterface.h"
#include "mt310s2senseinterface.h"
#include "resmanrunfacade.h"
#include "xmlhelperfortest.h"
#include "proxy.h"
#include "reply.h"
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
}

void test_regression_sense_interface_mt310s2::init()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceMt310s2>();
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_sense_interface_mt310s2::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_sense_interface_mt310s2::checkVersionsOfSystemInterface()
{
    QCOMPARE(m_mockServer->getDeviceVersion(), "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
}

void test_regression_sense_interface_mt310s2::checkExportXml()
{
    QString xmlExported = m_mockServer->getSenseInterface()->exportXMLString();
    qInfo("Exported XML (before adjust):");
    qInfo("%s", qPrintable(xmlExported));
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QFile xmlFile(":/regression_data/sense_interface_export.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    qInfo("Expected XML (before adjust):");
    qInfo("%s", qPrintable(xmlExpected));
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    // if this turns fragile we have to use zera-scpi's xml-compare-testlib
    QCOMPARE(xmlExported, xmlExpected);
}

QStringList test_regression_sense_interface_mt310s2::m_channelsExpectedAllOverThePlace = QStringList()
    << "m0" << "m1" << "m2" << "m6" << "m3" << "m4" << "m5" << "m7";


void test_regression_sense_interface_mt310s2::checkChannelCatalogAsExpected()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getChannelList();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2].toStringList(), m_channelsExpectedAllOverThePlace);
}

QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedU = QStringList()
    << "250V" << "8V" << "100mV";

void test_regression_sense_interface_mt310s2::checkRangesUL1()
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

// Is that a good idea to put it on modules's to hide internal current (voltage) ranges???
QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI = QStringList()
                                                                         << "10A" << "5A" << "2.5A" << "1.0A"
                                                                         << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI_Internal = QStringList()
                                                                         << "8V" << "5V" << "2V" << "1V"
                                                                         << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";

void test_regression_sense_interface_mt310s2::checkRangesIL1()
{
    SenseSystem::cChannelSettings *channelSetting = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("IL1");

    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal);
}

QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI_CL120A = QStringList()
                                                                                << "C100A" << "C50A" << "C10A" << "C5A" << "C1A"
                                                                                << "C500mA" << "C100mA" << "C50mA" << "C10mA";


void test_regression_sense_interface_mt310s2::addClampIL1_CL120A()
{
    SenseSystem::cChannelSettings *channelSettingI = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("IL1");
    SenseSystem::cChannelSettings *channelSettingU = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("UL1");

    ClampFactoryTest::setTestClampType(CL120A);
    cClampInterface* clampInterface = m_mockServer->getClampInterface();
    clampInterface->addClamp(channelSettingI, I2cMultiplexerFactory::createNullMuxer());

    QSignalSpy responseSpyI(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpyI.count(), 1);
    QCOMPARE(responseSpyI[0][0], QVariant(msgNr));
    QCOMPARE(responseSpyI[0][1], QVariant(ack));
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal + m_rangesExpectedI_CL120A);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    msgNr = m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpyU.count(), 1);
    QCOMPARE(responseSpyU[0][0], QVariant(msgNr));
    QCOMPARE(responseSpyU[0][1], QVariant(ack));
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU);
}

QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedU_CL800ADC1000VDC = QStringList() << "C1000V";
QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI_CL800ADC1000VDC = QStringList()
                                                                                         << "C800A" << "C400A" << "C200A" << "C80A" << "C40A" << "C20A";


void test_regression_sense_interface_mt310s2::addClampIL2_CL800ADC1000VDC()
{
    SenseSystem::cChannelSettings *channelSettingI = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("IL2");
    SenseSystem::cChannelSettings *channelSettingU = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("UL2");

    ClampFactoryTest::setTestClampType(CL800ADC1000VDC);
    cClampInterface* clampInterface = m_mockServer->getClampInterface();
    clampInterface->addClamp(channelSettingI, I2cMultiplexerFactory::createNullMuxer());

    QSignalSpy responseSpyI(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpyI.count(), 1);
    QCOMPARE(responseSpyI[0][0], QVariant(msgNr));
    QCOMPARE(responseSpyI[0][1], QVariant(ack));
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI + m_rangesExpectedI_Internal + m_rangesExpectedI_CL800ADC1000VDC);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    msgNr = m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpyU.count(), 1);
    QCOMPARE(responseSpyU[0][0], QVariant(msgNr));
    QCOMPARE(responseSpyU[0][1], QVariant(ack));
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU + m_rangesExpectedU_CL800ADC1000VDC);
}


QStringList test_regression_sense_interface_mt310s2::m_rangesExpectedI_DummyAux = QStringList() << "0A";

void test_regression_sense_interface_mt310s2::addRemoveClampIAUX_CL800ADC1000VDC()
{
    SenseSystem::cChannelSettings *channelSettingI = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("IAUX");
    SenseSystem::cChannelSettings *channelSettingU = m_mockServer->getSenseSettings()->findChannelSettingByAlias1("UAUX");

    // add
    ClampFactoryTest::setTestClampType(CL800ADC1000VDC);
    cClampInterface* clampInterface = m_mockServer->getClampInterface();
    clampInterface->addClamp(channelSettingI, I2cMultiplexerFactory::createNullMuxer());

    QSignalSpy responseSpyI1(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyI1[0][2].toStringList(), m_rangesExpectedI_DummyAux + m_rangesExpectedI_Internal + m_rangesExpectedI_CL800ADC1000VDC);

    QSignalSpy responseSpyU1(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpyU1[0][2].toStringList(), m_rangesExpectedU + m_rangesExpectedU_CL800ADC1000VDC);

    // remove - to have as much production code as possible we use actualizeClampStatus
    clampInterface->actualizeClampStatus(0);

    QSignalSpy responseSpyI(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getRangeList(channelSettingI->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpyI.count(), 1);
    QCOMPARE(responseSpyI[0][0], QVariant(msgNr));
    QCOMPARE(responseSpyI[0][1], QVariant(ack));
    QCOMPARE(responseSpyI[0][2].toStringList(), m_rangesExpectedI_DummyAux + m_rangesExpectedI_Internal);

    QSignalSpy responseSpyU(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    msgNr = m_pcbIFace->getRangeList(channelSettingU->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpyU.count(), 1);
    QCOMPARE(responseSpyU[0][0], QVariant(msgNr));
    QCOMPARE(responseSpyU[0][1], QVariant(ack));
    QCOMPARE(responseSpyU[0][2].toStringList(), m_rangesExpectedU);
}

void test_regression_sense_interface_mt310s2::genJsonRejectionValuesAllClampsIL3()
{
    genJsonConstantValuesAllRangesI("IL3");
}

void test_regression_sense_interface_mt310s2::genJsonRejectionValuesAllClampsIAUX()
{
    genJsonConstantValuesAllRangesI("IAUX");
}

void test_regression_sense_interface_mt310s2::genJsonRejectionValuesAllClampsUAUX()
{
    genJsonConstantValuesAllRangesI("UAUX", "IAUX");
}


QString test_regression_sense_interface_mt310s2::bareScpiQuery(QString bareScpiQuery)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(bareScpiQuery.toStdString());
    Zera::ProxyClientPtr proxylient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    QString bareScpiAnswer;
    connect(proxylient.get(), &Zera::ProxyClient::answerAvailable, this, [&](std::shared_ptr<ProtobufMessage::NetMessage> message) {
        bareScpiAnswer = QString::fromStdString(message->reply().body());
    });
    proxylient->transmitCommand(&envelope);
    TimeMachineObject::feedEventLoop();
    return bareScpiAnswer;
}

void test_regression_sense_interface_mt310s2::addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range)
{
    QString channelName = channelSettings->m_nameMx;

    QString avail = bareScpiQuery(QString("SENS:%1:%2:AVA?").arg(channelName, rangeName));
    range.insert("avail", avail);

    QString urValue = bareScpiQuery(QString("SENS:%1:%2:URV?").arg(channelName, rangeName));
    range.insert("urval", urValue);

    QString rejection = bareScpiQuery(QString("SENS:%1:%2:REJ?").arg(channelName, rangeName));
    range.insert("rejection", rejection);

    QString ovRejection = bareScpiQuery(QString("SENS:%1:%2:OVR?").arg(channelName, rangeName));
    range.insert("ovrejection", ovRejection);

    QString adcRejection = bareScpiQuery(QString("SENS:%1:%2:ADCR?").arg(channelName, rangeName));
    range.insert("adcrejection", adcRejection);
}

void test_regression_sense_interface_mt310s2::genJsonConstantValuesAllRangesI(QString channelName, QString channelNameAdRemoveClamps)
{
    if(channelNameAdRemoveClamps.isEmpty())
        channelNameAdRemoveClamps = channelName;
    SenseSystem::cChannelSettings *channelSetting = m_mockServer->getSenseSettings()->findChannelSettingByAlias1(channelName);
    SenseSystem::cChannelSettings *channelSettingClamps = m_mockServer->getSenseSettings()->findChannelSettingByAlias1(channelNameAdRemoveClamps);
    cClampInterface* clampInterface = m_mockServer->getClampInterface();
    QJsonObject jsonAll;
    for(int clampType=undefined+1; clampType<anzCL; clampType++) { // all clamp types
        // add
        ClampFactoryTest::setTestClampType(clampType);
        clampInterface->addClamp(channelSettingClamps, I2cMultiplexerFactory::createNullMuxer());
        QSignalSpy responseSpyI(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);

        m_pcbIFace->getRangeList(channelSetting->m_nameMx);
        TimeMachineObject::feedEventLoop();

        QJsonObject jsonRanges;
        const QStringList ranges = responseSpyI[0][2].toStringList();
        for(const QString &range : ranges) {
            QJsonObject jsonRange;
            addRangeConstantDataToJson(range, channelSetting, jsonRange);
            jsonRanges.insert(range, jsonRange);
        }

        // remove
        clampInterface->actualizeClampStatus(0);

        jsonAll.insert(cClamp::getClampTypeName(clampType), jsonRanges);
    }
    QJsonDocument doc(jsonAll);
    qInfo("----------------- rejection json generated %s -----------------", qPrintable(channelName));
    qInfo("%s", qPrintable(doc.toJson(QJsonDocument::Indented)));
}

