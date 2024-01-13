#include "test_config_load_mt310s2.h"
#include "xmlconfigreader.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_config_load_mt310s2)

void test_config_load_mt310s2::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_config_load_mt310s2::fileLoaded()
{
    Zera::XMLConfig::cReader configReader;
    QVERIFY(configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd"));
    QVERIFY(configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml"));
}
