#include "test_config_load_sec1000.h"
#include "xmlconfigreader.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_config_load_sec1000)

void test_config_load_sec1000::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_SEC1000D) + "/" + "sec1000d.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_config_load_sec1000::fileLoaded()
{
    Zera::XMLConfig::cReader configReader;
    QVERIFY(configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_SEC1000D) + "/" + "sec1000d.xml"));
}
