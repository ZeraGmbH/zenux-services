#include "test_regression_adj_import_export_xml_mt310s2.h"
#include "clamp.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "xmlhelperfortest.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <testloghelpers.h>
#include <mocktcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adj_import_export_xml_mt310s2);

void test_regression_adj_import_export_xml_mt310s2::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_regression_adj_import_export_xml_mt310s2::init()
{
    setupServers();
}

void test_regression_adj_import_export_xml_mt310s2::cleanup()
{
    m_proxyClient = nullptr;
    m_testServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_import_export_xml_mt310s2::directAcessExportXml()
{
    QString xmlExported = m_testServer->getSenseInterface()->exportXMLString();
    xmlExported = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExported);

    QString xmlExpected = TestLogHelpers::loadFile(":/export_internal_initial.xml");
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

    QString xmlExpected = TestLogHelpers::loadFile(":/export_internal_initial.xml");
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExportedInitial));

    QString filenameShort = ":/import_modified";
    QVERIFY(QFile::exists(filenameShort + ".xml"));
    QVERIFY(m_testServer->getSenseInterface()->importAdjXMLFile(filenameShort));

    QString xmlExportedModified = m_testServer->getSenseInterface()->exportXMLString();
    xmlExportedModified = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExportedModified);

    xmlExpected = TestLogHelpers::loadFile(":/import_modified.xml");
    xmlExpected = XmlHelperForTest::removeTimeDependentEntriesFromXml(xmlExpected);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(xmlExpected, xmlExportedModified));
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
    QString xmlExpected = TestLogHelpers::loadFile(":/export_internal_initial.xml");
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

    QString xmlExpected = TestLogHelpers::loadFile(":/export_clamp_initial.xml");
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

    QString xmlExpected = TestLogHelpers::loadFile(":/export_clamp_initial.xml");
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
    m_testServer->addClamp(-1, "IL1");

    QString xmlExported = ScpiSingleTransactionBlocked::query("SYSTEM:ADJUSTMENT:CLAMP:XML?");
    xmlExported = XmlHelperForTest::prettify(xmlExported);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff("", xmlExported));
}

void test_regression_adj_import_export_xml_mt310s2::setupServers()
{
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(
        std::make_shared<TestFactoryI2cCtrl>(true),
        m_tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
