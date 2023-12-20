#include "test_regression_sense_interface_com5003.h"
#include "com5003adjustment.h"
#include "com5003senseinterface.h"
#include "i2csettings.h"
#include "mockpcbserver.h"
#include "resmanrunfacade.h"
#include "sensesettings.h"
#include "proxy.h"
#include "reply.h"
#include "pcbinterface.h"
#include <timemachineobject.h>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_sense_interface_com5003);

// for now here later maybe in more common place or merge into MockMt310s2d
class MockForSenseInterface  : public MockPcbServer
{
public:
    MockForSenseInterface();
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }
    Com5003SenseInterface *getSenseInterface() { return m_senseInterface.get(); }
    Com5003Adjustment *getAdjustment() { return m_adjustment.get(); }
private:
    std::unique_ptr<cI2CSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;

    std::unique_ptr<Com5003SenseInterface> m_senseInterface;

    std::unique_ptr<cSystemInfo> m_systemInfo;
    std::unique_ptr<Com5003Adjustment> m_adjustment;
};

MockForSenseInterface::MockForSenseInterface() :
    MockPcbServer("com5003d")
{
    m_i2cSettings = std::make_unique<cI2CSettings>(getConfigReader());

    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_i2cSettings.get(), m_senseSettings.get()});

    m_senseInterface = std::make_unique<Com5003SenseInterface>(getSCPIInterface(), getRmConnection(), &m_ethSettings, m_senseSettings.get());
    setResources(ResourcesList{m_senseInterface.get()});

    // no resources / scpi interfaces from here
    m_systemInfo = std::make_unique<cSystemInfo>();
    // On MT this is handled directly by sense interface.
    // Here we have to inject sense interface into Com5003Adjustment
    m_adjustment = std::make_unique<Com5003Adjustment>(m_systemInfo.get(), m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    m_adjustment->addAdjXMLObject(m_senseInterface.get());

    start();
}


void test_regression_sense_interface_com5003::checkVersionsOfSystemInterface()
{
    MockForSenseInterface mock;
    QCOMPARE(mock.getDeviceVersion(), "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
}

void test_regression_sense_interface_com5003::checkExportXml()
{
    ResmanRunFacade resman;
    MockForSenseInterface mock;
    TimeMachineObject::feedEventLoop();

    // Date time are empty. On change: see mt310s2 and add XmlHelperForTest::removeTimeDependentEntriesFromXml
    QString xmlExported = mock.getAdjustment()->exportXMLString();
    qInfo("Exported XML:");
    qInfo("%s", qPrintable(xmlExported));

    QFile xmlFile(":/xmlfiles/adjustment_export.xml");
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
