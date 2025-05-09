#include "test_justdata.h"
#include "protonetcommand.h"
#include "zscpi_response_definitions.h"

QTEST_MAIN(test_justdata);


static constexpr int digits = 4;

void test_justdata::init()
{
    m_scpiInterface = std::make_shared<cSCPI>();
    m_adjDataGroup = new AdjDataRange;
    justData = new AdjDataItemScpi({m_scpiInterface, [] (bool &enable) { enable =true; return true;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justData->initSCPIConnection("sens:m0:8V:corr:offset");
}

void test_justdata::cleanup()
{
    delete justData;
    delete m_adjDataGroup;
}

void test_justdata::findScpiObject()
{
    QString scpiString = "sens:m0:8V:corr:offset:node:0?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiString);
    QVERIFY(scpiObject != nullptr);
}

void test_justdata::nodeSetAndRead()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:node:0 0.1;0.02;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr protoCmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));

    QString scpiStringRead = "sens:m0:8V:corr:offset:node:0?";
    ScpiObjectPtr scpiObjectRead = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectRead != nullptr);
    ProtonetCommandPtr protoCmdRead = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringRead);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectRead.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdRead));
    QCOMPARE((protoCmdRead->m_sOutput), "0.1000;0.0200;");
}

void test_justdata::coeffSetAndRead()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:coef:0 0.1234569;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr protoCmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));

    QString scpiStringRead = "sens:m0:8V:corr:offset:coef:0?";
    ScpiObjectPtr scpiObjectRead = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectRead != nullptr);
    ProtonetCommandPtr protoCmdRead = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringRead);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectRead.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdRead));
    QCOMPARE((protoCmdRead->m_sOutput), "0.123457"); // no fixed digits (arg() defaults to 6 digit + rounding??)
}

void test_justdata::nodeSetReject()
{
    AdjDataItemScpi *justDataReject = new AdjDataItemScpi({m_scpiInterface, [] (bool &enable) { enable=false; return true;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justDataReject->initSCPIConnection("sens:m1:8V:corr:offset");

    QString scpiStringWrite = "sens:m1:8V:corr:offset:node:0 0.1;0.02;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr protoCmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_justdata::coeffSetReject()
{
    AdjDataItemScpi *justDataReject = new AdjDataItemScpi({m_scpiInterface, [] (bool &enable) { enable=false; return true;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justDataReject->initSCPIConnection("sens:m1:8V:corr:offset");

    QString scpiStringWrite = "sens:m1:8V:corr:offset:coef:0 0.1;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr protoCmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_justdata::nodeSetFail()
{
    AdjDataItemScpi *justDataReject = new AdjDataItemScpi({m_scpiInterface, [] (bool &enable) { enable=true; return false;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justDataReject->initSCPIConnection("sens:m1:8V:corr:offset");

    QString scpiStringWrite = "sens:m1:8V:corr:offset:node:0 0.1;0.02;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr protoCmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_justdata::coeffSetFail()
{
    AdjDataItemScpi *justDataReject = new AdjDataItemScpi({m_scpiInterface, [] (bool &enable) { enable=true; return false;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justDataReject->initSCPIConnection("sens:m1:8V:corr:offset");

    QString scpiStringWrite = "sens:m1:8V:corr:offset:coef:0 0.1;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr protoCmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_justdata::nodeSetOneCrap()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:node:0 0.01;bar;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr protoCmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::errval]);
}

void test_justdata::nodeSettwoCrap()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:node:0 foo;0.01;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr protoCmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::errval]);
}

void test_justdata::coefSetCrap()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:coef:0 foo;";
    ScpiObjectPtr scpiObjectWrite = m_scpiInterface->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    ProtonetCommandPtr cmdWrite = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiStringWrite);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObjectWrite.get());
    QVERIFY(scpiDelegate->executeSCPI(cmdWrite));
    QCOMPARE(cmdWrite->m_sOutput, ZSCPI::scpiAnswer[ZSCPI::errval]);
}
