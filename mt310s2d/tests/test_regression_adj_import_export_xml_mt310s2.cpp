#include "test_regression_adj_import_export_xml_mt310s2.h"
#include "clampfactorytest.h"
#include "proxy.h"
#include "atmel.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_xml_mt310s2);

void test_regression_adj_import_export_xml_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
}

void test_regression_adj_import_export_xml_mt310s2::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileExportXml()
{
    setupServers(&Atmel::getInstance());

    QString xmlExported = m_mockServer->getSenseInterface()->exportXMLString();
    qInfo("Exported XML (before adjust):");
    qInfo("%s", qPrintable(xmlExported));
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QFile xmlFile(":/export_inititial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    qInfo("Expected XML (before adjust):");
    qInfo("%s", qPrintable(xmlExpected));
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    // if this turns fragile we have to use zera-scpi's xml-compare-testlib
    QCOMPARE(xmlExported, xmlExpected);
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileImportXmlMinimal()
{
    setupServers(&Atmel::getInstance());

    QString filenameShort = ":/import_minimal_pass";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileImportXmlPseudoRandom()
{
    setupServers(&Atmel::getInstance());

    QString xmlExportedInitial = m_mockServer->getSenseInterface()->exportXMLString();
    xmlExportedInitial = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExportedInitial);
    QFile xmlFileInitial(":/export_inititial.xml");
    QVERIFY(xmlFileInitial.open(QFile::ReadOnly));
    QString xmlExpected = xmlFileInitial.readAll();
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);
    QCOMPARE(xmlExportedInitial, xmlExpected);

    QString filenameShort = ":/import_modified";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_mockServer->getSenseInterface()->importAdjXMLFile(filenameShort));

    QString xmlExportedModified = m_mockServer->getSenseInterface()->exportXMLString();
    xmlExportedModified = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExportedModified);
    QFile xmlFileModified(":/import_modified.xml");
    QVERIFY(xmlFileModified.open(QFile::ReadOnly));
    xmlExpected = xmlFileModified.readAll();
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);
    QCOMPARE(xmlExportedModified, xmlExpected);
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileImportMissingType()
{
    setupServers(&Atmel::getInstance());

    QString filenameShort = ":/import_missing_type";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_mockServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileImportMissingSerNo()
{
    setupServers(&Atmel::getInstance());

    QString filenameShort = ":/import_missing_serno";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_mockServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_mt310s2::scpiExportInitialAdjXml()
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

void test_regression_adj_import_export_xml_mt310s2::scpiImportPermissionQueryFail()
{
    setupServers(&Atmel::getInstance());

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_adj_import_export_xml_mt310s2::scpiImportNoPermission()
{
    AtmelPermissionTemplatePtrU perm = AtmelPermissionMock::createAlwaysDisabled();
    setupServers(perm.get());

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_regression_adj_import_export_xml_mt310s2::scpiImportInvalidXml()
{
    AtmelPermissionTemplatePtrU perm = AtmelPermissionMock::createAlwaysEnabled();
    setupServers(perm.get());

    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:XML", "foo");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::errxml]);
}

void test_regression_adj_import_export_xml_mt310s2::setupServers(AtmelPermissionTemplate *permissionQueryHandler)
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceMt310s2>(permissionQueryHandler);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
