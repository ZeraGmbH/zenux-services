#include "test_senserange.h"
#include "senserange.h"
#include "senseinterface.h"

#include <protonetcommand.h>
#include <scpi.h>


QTEST_MAIN(test_senserange);



void test_senserange::init()
{
    _init("SENSE:m4", "100mA", 1000, 1000, 1000, 1000, SenseSystem::modeAC);
}

void test_senserange::cleanup()
{
    _cleanup();
}

void test_senserange::findScpiObject()
{   
    QString scpiString = "SENSE:m4:100mA:REJECTION?";
    cSCPIObject* scpiObject = scpi->getSCPIObject(scpiString);
    QVERIFY(scpiObject != nullptr);
}

void test_senserange::executeScpiQuery()
{
    QString scpiString = "SENSE:m4:100mA:REJECTION?";
    cSCPIObject* scpiObject = scpi->getSCPIObject(scpiString);
    QVERIFY(scpiObject != nullptr);

    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiString);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));
}

void test_senserange::verifyScpiQuery()
{   
    QString scpiString = "SENSE:m4:100mA:REJECTION?";
    cSCPIObject* scpiObject = scpi->getSCPIObject(scpiString);
    QVERIFY(scpiObject != nullptr);

    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiString);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));
    QCOMPARE((protoCmd->m_sOutput), "1000");
}
