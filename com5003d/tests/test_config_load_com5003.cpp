#include "test_config_load_com5003.h"
#include "xmlconfigreader.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_config_load_com5003)

void test_config_load_com5003::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_COM5003D) + "/" + "com5003d.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_config_load_com5003::fileLoaded()
{
    Zera::XMLConfig::cReader configReader;
    QVERIFY(configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_COM5003D) + "/" + "com5003d.xsd"));
    QVERIFY(configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_COM5003D) + "/" + "com5003d.xml"));
}
