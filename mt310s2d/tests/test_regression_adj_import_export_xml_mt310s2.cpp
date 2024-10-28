#include "test_regression_adj_import_export_xml_mt310s2.h"
#include "clampfactorytest.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <tcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_xml_mt310s2);

void test_regression_adj_import_export_xml_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
}

void test_regression_adj_import_export_xml_mt310s2::init()
{
    setupServers();
}

void test_regression_adj_import_export_xml_mt310s2::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_xml_mt310s2::directAcessExportXml()
{
    QString xmlExported = m_testServer->getSenseInterface()->exportXMLString();
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QFile xmlFile(":/export_internal_initial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileImportXmlMinimal()
{
    QString filenameShort = ":/import_minimal_pass";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_mt310s2::directAcessArbitraryVerion()
{
    QString filenameShort = ":/import_abribrary_version";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileImportXmlPseudoRandom()
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
    QCOMPARE(xmlExportedModified, xmlExpected);
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileImportMissingType()
{
    QString filenameShort = ":/import_missing_type";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_mt310s2::directAcessFileImportMissingSerNo()
{
    QString filenameShort = ":/import_missing_serno";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_mt310s2::directAcessUnknownEntry()
{
    QString filenameShort = ":/import_unknown_entry";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(!m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));
}

void test_regression_adj_import_export_xml_mt310s2::scpiExportInitialAdjXml()
{
    QFile xmlFile(":/export_internal_initial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prettify(xmlExpected);
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:XML?");
    xmlExported = XmlHelperForTest::prettify(xmlExported);
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_xml_mt310s2::directAcessExportXmlClamps()
{
    m_testServer->addClamp(cClamp::CL120A, "IL1");
    m_testServer->addClamp(cClamp::EMOB32, "IL2");
    m_testServer->addClamp(cClamp::EMOB200DC, "IL3");
    m_testServer->addClamp(cClamp::CL200ADC1000VDC, "IAUX");

    QFile xmlFile(":/export_clamp_initial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prettify(xmlExpected);
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    QString xmlExported = m_testServer->getClampInterface()->exportXMLString(4);
    xmlExported = XmlHelperForTest::prettify(xmlExported);
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}


void test_regression_adj_import_export_xml_mt310s2::scpiExportInitialAdjXmlClamps()
{
    m_testServer->addClamp(cClamp::CL120A, "IL1");
    m_testServer->addClamp(cClamp::EMOB32, "IL2");
    m_testServer->addClamp(cClamp::EMOB200DC, "IL3");
    m_testServer->addClamp(cClamp::CL200ADC1000VDC, "IAUX");

    QFile xmlFile(":/export_clamp_initial.xml");
    QVERIFY(xmlFile.open(QFile::ReadOnly));
    QString xmlExpected = xmlFile.readAll();
    xmlExpected = XmlHelperForTest::prettify(xmlExpected);
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:CLAMP:XML?");
    xmlExported = XmlHelperForTest::prettify(xmlExported);
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExported));
}

void test_regression_adj_import_export_xml_mt310s2::scpiExportUndefinedClamp()
{
    m_testServer->addClamp(cClamp::undefined, "IL1");

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:CLAMP:XML?");
    xmlExported = XmlHelperForTest::prettify(xmlExported);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff("", xmlExported));
}

void test_regression_adj_import_export_xml_mt310s2::scpiExportInvalidClamp()
{
    m_testServer->addClamp(cClamp::ClampTypes(-1), "IL1");

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:CLAMP:XML?");
    xmlExported = XmlHelperForTest::prettify(xmlExported);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff("", xmlExported));
}

void test_regression_adj_import_export_xml_mt310s2::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
