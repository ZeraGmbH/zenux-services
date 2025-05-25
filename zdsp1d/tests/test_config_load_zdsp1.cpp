#include "test_config_load_zdsp1.h"
#include "xmlconfigreader.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_config_load_zdsp1)

void test_config_load_zdsp1::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_ZDSP1D) + "/" + "zdsp1d.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_config_load_zdsp1::fileLoaded()
{
    Zera::XMLConfig::cReader configReader;
    QVERIFY(configReader.loadXMLFile(QStringLiteral(CONFIG_SOURCES_ZDSP1D) + "/" + "zdsp1d.xml"));
}
