#include "test_regression_adj_import_export_xml_com5003.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "xmlhelperfortest.h"
#include "testfactoryi2cctrl.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <tcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_xml_com5003);

void test_regression_adj_import_export_xml_com5003::init()
{
    setupServers();
}

void test_regression_adj_import_export_xml_com5003::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_xml_com5003::directAcessExportXml()
{
    QString xmlExported = m_testServer->getSenseInterface()->exportXMLString();
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QFile xmlFile(":/export_internal_initial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_xml_com5003::directAcessFileImportXmlMinimal()
{
    QString filenameShort = ":/import_minimal_pass";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::directAcessArbitraryVerion()
{
    QString filenameShort = ":/import_abribrary_version";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::directAcessFileImportXmlPseudoRandom()
{
    QString xmlExportedInitial = m_testServer->getSenseInterface()->exportXMLString();
    xmlExportedInitial = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExportedInitial);
    QFile xmlFileInitial(":/export_internal_initial.xml");
    QVERIFY(xmlFileInitial.open(QFile::ReadOnly));
    QString xmlExpected = xmlFileInitial.readAll();
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);
    QCOMPARE(xmlExportedInitial, xmlExpected);

    QString filenameShort = ":/import_modified";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));

    QString xmlExportedModified = m_testServer->getSenseInterface()->exportXMLString();
    xmlExportedModified = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExportedModified);
    QFile xmlFileModified(":/import_modified.xml");
    QVERIFY(xmlFileModified.open(QFile::ReadOnly));
    xmlExpected = xmlFileModified.readAll();
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExportedModified));
}

void test_regression_adj_import_export_xml_com5003::directAcessFileImportMissingType()
{
    QString filenameShort = ":/import_missing_type";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::directAcessFileImportMissingSerNo()
{
    QString filenameShort = ":/import_missing_serno";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::directAcessUnknownEntry()
{
    QString filenameShort = ":/import_unknown_entry";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_com5003::scpiExportInitialAdjXml()
{
    QFile xmlFile(":/export_internal_initial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prettify(xmlExpected);
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?");
    xmlExported = XmlHelperForTest::prettify(xmlExported);
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    // if this turns fragile we have to use zera-scpi's xml-compare-testlib
    QCOMPARE(xmlExported, xmlExpected);
}

void test_regression_adj_import_export_xml_com5003::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceCom5003>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
