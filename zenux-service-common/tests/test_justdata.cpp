#include "test_justdata.h"
#include "protonetcommand.h"
#include "zscpi_response_definitions.h"

QTEST_MAIN(test_justdata);


static constexpr int digits = 4;

void test_justdata::init()
{
    scpi = new cSCPI();
    m_adjDataGroup = new AdjDataRangeGroup;
    justData = new AdjDataItemInterface({scpi, 5, 0.1, [] (bool &enable) { enable =true; return true;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justData->initSCPIConnection("sens:m0:8V:corr:offset");
}

void test_justdata::cleanup()
{
    delete justData;
    delete m_adjDataGroup;
    delete scpi;
}

void test_justdata::findScpiObject()
{
    QString scpiString = "sens:m0:8V:corr:offset:node:0?";
    cSCPIObject* scpiObject = scpi->getSCPIObject(scpiString);
    QVERIFY(scpiObject != nullptr);
}

void test_justdata::nodeSetAndRead()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:node:0 0.1;0.02;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));

    QString scpiStringRead = "sens:m0:8V:corr:offset:node:0?";
    cSCPIObject* scpiObjectRead = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectRead != nullptr);
    cProtonetCommand* protoCmdRead = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringRead);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectRead);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdRead));
    QCOMPARE((protoCmdRead->m_sOutput), "0.1000;0.0200;");
}

void test_justdata::coeffSetAndRead()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:coef:0 0.1234569;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));

    QString scpiStringRead = "sens:m0:8V:corr:offset:coef:0?";
    cSCPIObject* scpiObjectRead = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectRead != nullptr);
    cProtonetCommand* protoCmdRead = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringRead);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectRead);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdRead));
    QCOMPARE((protoCmdRead->m_sOutput), "0.123457"); // no fixed digits (arg() defaults to 6 digit + rounding??)
}

void test_justdata::nodeSetReject()
{
    AdjDataItemInterface *justDataReject = new AdjDataItemInterface({scpi, 5, 0.1, [] (bool &enable) { enable=false; return true;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justDataReject->initSCPIConnection("sens:m1:8V:corr:offset");

    QString scpiStringWrite = "sens:m1:8V:corr:offset:node:0 0.1;0.02;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_justdata::coeffSetReject()
{
    AdjDataItemInterface *justDataReject = new AdjDataItemInterface({scpi, 5, 0.1, [] (bool &enable) { enable=false; return true;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justDataReject->initSCPIConnection("sens:m1:8V:corr:offset");

    QString scpiStringWrite = "sens:m1:8V:corr:offset:coef:0 0.1;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::erraut]);
}

void test_justdata::nodeSetFail()
{
    AdjDataItemInterface *justDataReject = new AdjDataItemInterface({scpi, 5, 0.1, [] (bool &enable) { enable=true; return false;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justDataReject->initSCPIConnection("sens:m1:8V:corr:offset");

    QString scpiStringWrite = "sens:m1:8V:corr:offset:node:0 0.1;0.02;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_justdata::coeffSetFail()
{
    AdjDataItemInterface *justDataReject = new AdjDataItemInterface({scpi, 5, 0.1, [] (bool &enable) { enable=true; return false;}, digits}, &m_adjDataGroup->m_gainAdjData);
    justDataReject->initSCPIConnection("sens:m1:8V:corr:offset");

    QString scpiStringWrite = "sens:m1:8V:corr:offset:coef:0 0.1;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_justdata::nodeSetOneCrap()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:node:0 0.01;bar;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::errval]);
}

void test_justdata::nodeSettwoCrap()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:node:0 foo;0.01;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));
    QCOMPARE((protoCmdWrite->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::errval]);
}

void test_justdata::coefSetCrap()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:coef:0 foo;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand cmdWrite(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(&cmdWrite));
    QCOMPARE(cmdWrite.m_sOutput, ZSCPI::scpiAnswer[ZSCPI::errval]);
}
