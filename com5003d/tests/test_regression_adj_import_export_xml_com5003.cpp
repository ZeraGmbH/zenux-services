#include "test_regression_adj_import_export_xml_com5003.h"
#include "proxy.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_xml_com5003);

void test_regression_adj_import_export_xml_com5003::init()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceCom5003>();
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_xml_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_xml_com5003::checkExportXml()
{
    // Date time are empty. On change: see mt310s2 and add XmlHelperForTest::removeTimeDependentEntriesFromXml
    QString xmlExported = m_mockServer->getAdjustment()->exportXMLString();
    qInfo("Exported XML:");
    qInfo("%s", qPrintable(xmlExported));

    QFile xmlFile(":/export_inititial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    qInfo("Expected XML:");
    qInfo("%s", qPrintable(xmlExpected));

    // if this turns fragile we have to use zera-scpi's xml-compare-testlib
    QCOMPARE(xmlExported, xmlExpected);
}

void test_regression_adj_import_export_xml_com5003::checkImportXmlMinimal()
{
    QString filenameShort = ":/import_minimal_pass";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::checkImportXmlFull()
{
    QString filenameShort = ":/export_inititial";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::checkImportMissingType()
{
    QString filenameShort = ":/import_missing_type";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    // Currently there is no type check -> pass
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::checkImportMissingSerNo()
{
    QString filenameShort = ":/import_missing_serno";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

