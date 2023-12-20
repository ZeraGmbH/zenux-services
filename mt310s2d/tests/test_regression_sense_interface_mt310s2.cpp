#include "test_regression_sense_interface_mt310s2.h"
#include "clampinterface.h"
#include "mockpcbserver.h"
#include "i2csettings.h"
#include "mt310s2senseinterface.h"
#include "resmanrunfacade.h"
#include "sensesettings.h"
#include "systeminfo.h"
#include "xmlhelperfortest.h"
#include "proxy.h"
#include "reply.h"
#include "pcbinterface.h"
#include "clampfactorytest.h"
#include <i2cmultiplexerfactory.h>
#include <timemachineobject.h>
#include <QFile>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_sense_interface_mt310s2);

// for now here later maybe in more common place or merge into MockMt310s2d
class MockForSenseInterface  : public MockPcbServer
{
public:
    MockForSenseInterface();
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }
    Mt310s2SenseInterface* getSenseInterface() { return m_senseInterface.get(); }
    cClampInterface* getClampInterface() { return m_clampInterface.get(); }
    cSenseSettings* getSenseSettings() { return m_senseSettings.get(); }
private:
    std::unique_ptr<cI2CSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;
    std::unique_ptr<cSystemInfo> m_systemInfo;
    std::unique_ptr<Mt310s2SenseInterface> m_senseInterface;
    std::unique_ptr<cClampInterface> m_clampInterface;
};

MockForSenseInterface::MockForSenseInterface() :
    MockPcbServer("mt310s2d")
{
    m_i2cSettings = std::make_unique<cI2CSettings>(getConfigReader());
    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_i2cSettings.get(), m_senseSettings.get()});

    m_systemInfo = std::make_unique<cSystemInfo>();

    m_senseInterface = std::make_unique<Mt310s2SenseInterface>(getSCPIInterface(), m_i2cSettings.get(), m_senseSettings.get(), m_systemInfo.get());
    m_clampInterface = std::make_unique<cClampInterface>(this, m_i2cSettings.get(), m_senseSettings.get(), m_senseInterface.get());
    setResources(ResourcesList{m_senseInterface.get()});
    start();
}


void test_regression_sense_interface_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
}

void test_regression_sense_interface_mt310s2::checkVersionsOfSystemInterface()
{
    MockForSenseInterface mock;
    QCOMPARE(mock.getDeviceVersion(), "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
}

void test_regression_sense_interface_mt310s2::checkExportXml()
{
    ResmanRunFacade resman;
    MockForSenseInterface mock;
    TimeMachineObject::feedEventLoop();

    QString xmlExported = mock.getSenseInterface()->exportXMLString();
    qInfo("Exported XML (before adjust):");
    qInfo("%s", qPrintable(xmlExported));
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QFile xmlFile(":/xmlfiles/sense_interface_export.xml");
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
    ResmanRunFacade resman;
    MockForSenseInterface mock;
    TimeMachineObject::feedEventLoop();

    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(pcbClient);

    Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    TimeMachineObject::feedEventLoop();

    QSignalSpy responseSpy(&pcbIFace, &Zera::cPCBInterface::serverAnswer);
    int msgNr = pcbIFace.getChannelList();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2].toStringList(), m_channelsExpectedAllOverThePlace);
}

constexpr int mt310s2PhaseCount = 4; // can we get this from config please?

void test_regression_sense_interface_mt310s2::addClampIL1()
{
    ResmanRunFacade resman;
    MockForSenseInterface mock;
    TimeMachineObject::feedEventLoop();

    cClampInterface* clampInterface = mock.getClampInterface();
    SenseSystem::cChannelSettings *channelSetting = mock.getSenseSettings()->findChannelSettingByAlias1("IL1");
    clampInterface->addClamp(channelSetting->m_nCtrlChannel,
                             I2cMultiplexerFactory::createNullMuxer(),
                             channelSetting->m_nPluggedBit,
                             mt310s2PhaseCount,
                             channelSetting->m_nameMx);
}

