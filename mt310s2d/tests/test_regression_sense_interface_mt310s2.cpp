#include "test_regression_sense_interface_mt310s2.h"
#include "mockpcbserver.h"
#include "i2csettings.h"
#include "mt310s2senseinterface.h"
#include "resmanrunfacade.h"
#include "sensesettings.h"
#include "systeminfo.h"
#include <timemachineobject.h>
#include <QFile>
#include <QRegularExpression>
#include <QTest>

QTEST_MAIN(test_regression_sense_interface_mt310s2);

// for now here later maybe in more common place or merge into MockMt310s2d
class MockForSenseInterface  : public MockPcbServer
{
public:
    MockForSenseInterface();
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }
    Mt310s2SenseInterface* getSenseInterface() { return m_senseInterface.get(); }
private:
    std::unique_ptr<cI2CSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;
    std::unique_ptr<cSystemInfo> m_systemInfo;
    std::unique_ptr<Mt310s2SenseInterface> m_senseInterface;
};

MockForSenseInterface::MockForSenseInterface() :
    MockPcbServer("mt310s2d")
{
    m_i2cSettings = std::make_unique<cI2CSettings>(getConfigReader());
    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_i2cSettings.get(), m_senseSettings.get()});

    m_systemInfo = std::make_unique<cSystemInfo>();

    m_senseInterface = std::make_unique<Mt310s2SenseInterface>(getSCPIInterface(), m_i2cSettings.get(), m_senseSettings.get(), m_systemInfo.get());
    setResources(ResourcesList{m_senseInterface.get()});
    start();
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

    static QRegularExpression regexDate("<Date>.*<\\/Date>");
    xmlExported = xmlExported.replace(regexDate, "<Date>nodate</Date>");
    static QRegularExpression regexTime("<Time>.*<\\/Time>");
    xmlExported = xmlExported.replace(regexTime, "<Time>notime</Time>");
    static QRegularExpression regexChecksum("<Chksum>.*<\\/Chksum>");
    xmlExported = xmlExported.replace(regexChecksum, "<Chksum>nocheck</Chksum>");
    qInfo("%s", qPrintable(xmlExported));

    QFile xmlFile(":/xmlfiles/range_interface_export.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();

    // if this turns fragile we have to use zera-scpi's xml-compare-testlib
    QCOMPARE(xmlExported, xmlExpected);
}

