#include "test_mt310s2_config_load.h"
#include "xmlconfigreader.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mt310s2_config_load)

void test_mt310s2_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml");
    qInfo("%s", qPrintable(tmpXmlConfigFile.fileName()));
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_mt310s2_config_load::fileLoaded()
{
    Zera::XMLConfig::cReader configReader;
    QVERIFY(configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd"));
    QVERIFY(configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml"));
}
