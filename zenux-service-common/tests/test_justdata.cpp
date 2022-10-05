#include "test_justdata.h"
#include <protonetcommand.h>

QTEST_MAIN(test_justdata);


static constexpr int digits = 4;

void test_justdata::init()
{
    scpi = new cSCPI("foo");
    justData = new cJustData({scpi, 5, 0.1, [] (bool &enable) { enable =true; return true;}, digits});
    justData->initSCPIConnection("sens:m0:8V:corr:offset");
}

void test_justdata::cleanup()
{
    delete justData;
    delete scpi;
}

void test_justdata::findScpiObject()
{
    QString scpiString = "sens:m0:8V:corr:offset:node:0?";
    cSCPIObject* scpiObject = scpi->getSCPIObject(scpiString, false);
    QVERIFY(scpiObject != nullptr);
}

void test_justdata::nodeSetAndRead()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:node:0 0.1;0.02;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite, false);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));

    QString scpiStringRead = "sens:m0:8V:corr:offset:node:0?";
    cSCPIObject* scpiObjectRead = scpi->getSCPIObject(scpiStringWrite, false);
    QVERIFY(scpiObjectRead != nullptr);
    cProtonetCommand* protoCmdRead = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringRead);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectRead);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdRead));
    QCOMPARE((protoCmdRead->m_sOutput), "0.1000;0.0200;");
}

void test_justdata::coeffSetAndRead()
{
    QString scpiStringWrite = "sens:m0:8V:corr:offset:coef:0 0.1234569;";
    cSCPIObject* scpiObjectWrite = scpi->getSCPIObject(scpiStringWrite, false);
    QVERIFY(scpiObjectWrite != nullptr);
    cProtonetCommand* protoCmdWrite = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringWrite);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectWrite);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdWrite));

    QString scpiStringRead = "sens:m0:8V:corr:offset:coef:0?";
    cSCPIObject* scpiObjectRead = scpi->getSCPIObject(scpiStringWrite, false);
    QVERIFY(scpiObjectRead != nullptr);
    cProtonetCommand* protoCmdRead = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiStringRead);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObjectRead);
    QVERIFY(scpiDelegate->executeSCPI(protoCmdRead));
    QCOMPARE((protoCmdRead->m_sOutput), "0.123457"); // no fixed digits (arg() defaults to 6 digit + rounding??)
}
