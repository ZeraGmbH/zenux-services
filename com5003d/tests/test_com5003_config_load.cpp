#include "test_com5003_config_load.h"
#include "xmlconfigreader.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_com5003_config_load)

void test_com5003_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_COM5003D) + "/" + "com5003d.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_com5003_config_load::fileLoaded()
{
    Zera::XMLConfig::cReader configReader;
    QVERIFY(configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_COM5003D) + "/" + "com5003d.xsd"));
    QVERIFY(configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_COM5003D) + "/" + "com5003d.xml"));
}
