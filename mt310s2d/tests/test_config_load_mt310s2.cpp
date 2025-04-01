#include "test_config_load_mt310s2.h"
#include "settingscontainer.h"
#include "xmlconfigreader.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_config_load_mt310s2)

void test_config_load_mt310s2::initTestCase_data()
{
    QTest::addColumn<QString>("serviceName");
    QTest::newRow("mt310s2d") << QString("mt310s2d");
    QTest::newRow("mt581s2d") << QString("mt581s2d");
}

void test_config_load_mt310s2::fileFound()
{
    QFETCH_GLOBAL(QString, serviceName);
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceName);
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + config.xmlFileName);
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_config_load_mt310s2::fileLoaded()
{
    QFETCH_GLOBAL(QString, serviceName);
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceName);
    Zera::XMLConfig::cReader configReader;
    QVERIFY(configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + config.xsdFileName));
    QVERIFY(configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + config.xmlFileName));
}

