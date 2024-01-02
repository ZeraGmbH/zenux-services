#include "test_regression_adj_import_export_xml_com5003.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include "atmel.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_xml_com5003);

void test_regression_adj_import_export_xml_com5003::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_xml_com5003::directAcessFileExportXml()
{
    setupServers(&Atmel::getInstance());

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

void test_regression_adj_import_export_xml_com5003::directAcessFileImportXmlMinimal()
{
    setupServers(&Atmel::getInstance());

    QString filenameShort = ":/import_minimal_pass";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::directAcessFileImportXmlPseudoRandom()
{
    setupServers(&Atmel::getInstance());

    QString xmlExportedInitial = m_mockServer->getAdjustment()->exportXMLString();
    QFile xmlFileInitial(":/export_inititial.xml");
    QVERIFY(xmlFileInitial.open(QFile::ReadOnly));
    QString xmlExpected = xmlFileInitial.readAll();
    QCOMPARE(xmlExportedInitial, xmlExpected);

    QString filenameShort = ":/import_modified";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));

    QString xmlExportedModified = m_mockServer->getAdjustment()->exportXMLString();
    QFile xmlFileModified(":/import_modified.xml");
    QVERIFY(xmlFileModified.open(QFile::ReadOnly));
    xmlExpected = xmlFileModified.readAll();
    QCOMPARE(xmlExportedModified, xmlExpected);
}

void test_regression_adj_import_export_xml_com5003::directAcessFileImportMissingType()
{
    setupServers(&Atmel::getInstance());

    QString filenameShort = ":/import_missing_type";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    // Currently there is no type check -> pass
    QVERIFY(m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::directAcessFileImportMissingSerNo()
{
    setupServers(&Atmel::getInstance());

    QString filenameShort = ":/import_missing_serno";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_mockServer->getAdjustment()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::scpiExportInitialAdjXml()
{
    setupServers(&Atmel::getInstance());

    QFile xmlFile(":/export_inititial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    qInfo("Expected XML:");
    qInfo("%s", qPrintable(xmlExpected));
    xmlExpected = XmlHelperForTest::prettify(xmlExpected);
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?");
    QString xmlExportedPretty = XmlHelperForTest::prettify(xmlExported);
    qInfo("Exported XML:");
    qInfo("%s", qPrintable(xmlExportedPretty));
    xmlExportedPretty = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExportedPretty);

    // if this turns fragile we have to use zera-scpi's xml-compare-testlib
    QCOMPARE(xmlExportedPretty, xmlExpected);
}

void test_regression_adj_import_export_xml_com5003::scpiImportPermissionQueryFail()
{
    setupServers(&Atmel::getInstance());

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_adj_import_export_xml_com5003::setupServers(AtmelPermissionTemplate *permissionQueryHandler)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceCom5003>(permissionQueryHandler);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

