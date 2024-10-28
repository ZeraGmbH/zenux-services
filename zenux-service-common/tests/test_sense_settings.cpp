#include "test_sense_settings.h"
#include "testpcbserver.h"
#include "sensesettings.h"
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_sense_settings);

class MockForSenseSettings  : public TestPcbServer
{
public:
    MockForSenseSettings(QString deamonName, VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    cSenseSettings* getSenseSettings() { return m_senseSettings.get(); }
private:
    std::unique_ptr<cSenseSettings> m_senseSettings;
};

MockForSenseSettings::MockForSenseSettings(QString deamonName, VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory) :
    TestPcbServer(deamonName, tcpNetworkFactory)
{
    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_senseSettings.get()});
    start();
}

void test_sense_settings::findByAlias1Com5003()
{
    MockForSenseSettings mock("com5003d", VeinTcp::MockTcpNetworkFactory::create());
    cSenseSettings* senseSettings = mock.getSenseSettings();
    // we have this assumption on config all over the place
    QCOMPARE(senseSettings->findChannelSettingByAlias1("UL1")->m_nameMx, "m0");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("UL2")->m_nameMx, "m1");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("UL3")->m_nameMx, "m2");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("IL1")->m_nameMx, "m3");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("IL2")->m_nameMx, "m4");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("IL3")->m_nameMx, "m5");
}

void test_sense_settings::findByAlias1Mt310s2()
{
    MockForSenseSettings mock("mt310s2d", VeinTcp::MockTcpNetworkFactory::create());
    cSenseSettings* senseSettings = mock.getSenseSettings();
    // we have this assumption on config all over the place
    QCOMPARE(senseSettings->findChannelSettingByAlias1("UL1")->m_nameMx, "m0");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("UL2")->m_nameMx, "m1");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("UL3")->m_nameMx, "m2");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("IL1")->m_nameMx, "m3");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("IL2")->m_nameMx, "m4");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("IL3")->m_nameMx, "m5");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("UAUX")->m_nameMx, "m6");
    QCOMPARE(senseSettings->findChannelSettingByAlias1("IAUX")->m_nameMx, "m7");
}

void test_sense_settings::findByInMxCom5003()
{
    MockForSenseSettings mock("com5003d", VeinTcp::MockTcpNetworkFactory::create());
    cSenseSettings* senseSettings = mock.getSenseSettings();
    // we have this assumption on config all over the place
    QCOMPARE(senseSettings->findChannelSettingByMxName("m0")->m_nameMx, "m0");
    QCOMPARE(senseSettings->findChannelSettingByMxName("m1")->m_nameMx, "m1");
    QCOMPARE(senseSettings->findChannelSettingByMxName("m2")->m_nameMx, "m2");
    QCOMPARE(senseSettings->findChannelSettingByMxName("m3")->m_nameMx, "m3");
    QCOMPARE(senseSettings->findChannelSettingByMxName("m4")->m_nameMx, "m4");
    QCOMPARE(senseSettings->findChannelSettingByMxName("m5")->m_nameMx, "m5");
}
