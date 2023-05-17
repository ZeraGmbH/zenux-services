#include "test_zdsp1_config_load.h"
#include "xmlconfigreader.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_zdsp1_config_load)

void test_zdsp1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_ZDSP1D) + "/" + "zdsp1d.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_zdsp1_config_load::fileLoaded()
{
    Zera::XMLConfig::cReader configReader;
    QVERIFY(configReader.loadSchema(QStringLiteral(CONFIG_SOURCES_ZDSP1D) + "/" + "zdsp1d.xsd"));
    QVERIFY(configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_ZDSP1D) + "/" + "zdsp1d.xml"));
}
