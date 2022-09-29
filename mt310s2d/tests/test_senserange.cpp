#include "test_senserange.h"
#include "senserange.h"
#include "senseinterface.h"

#include <protonetcommand.h>
#include <scpi.h>


QTEST_MAIN(test_senserange);

void test_senserange::findScpiObject()
{
    cSCPI scpi("foo");
    cMT310S2JustData *justData = new cMT310S2JustData(&scpi);
    cSenseRange testRange(&scpi, "100mA", "100mA", true, 1000, 1000, 1000, 1000, 0, SenseSystem::modeAC, justData);
    testRange.initSCPIConnection("SENSE:m4");

    QString scpiString = "SENSE:m4:100mA:REJECTION?";
    cSCPIObject* scpiObject = scpi.getSCPIObject(scpiString, false);
    QVERIFY(scpiObject != nullptr);
}

void test_senserange::executeScpiQuery()
{
    cSCPI scpi("foo");
    cMT310S2JustData *justData = new cMT310S2JustData(&scpi);
    cSenseRange testRange(&scpi, "100mA", "100mA", true, 1000, 1000, 1000, 1000, 0, SenseSystem::modeAC, justData);
    testRange.initSCPIConnection("SENSE:m4");

    QString scpiString = "SENSE:m4:100mA:REJECTION?";
    cSCPIObject* scpiObject = scpi.getSCPIObject(scpiString, false);
    QVERIFY(scpiObject != nullptr);

    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiString);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    QVERIFY(scpiDelegate->executeSCPI(protoCmd));
}
