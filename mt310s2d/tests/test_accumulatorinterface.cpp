#include "test_accumulatorinterface.h"
#include "accumulatorinterface.h"
#include <QTest>

QTEST_MAIN(test_accumulatorinterface);
constexpr char *systemAccumulatorStatus ="SYSTEM:ACCUMULATOR:STATUS?";

void test_accumulatorinterface::Compile()
{
    cSCPI scpiInterface("");
    AccumulatorInterface accumulator(&scpiInterface);
    accumulator.initSCPIConnection("");
    QVERIFY(scpiInterface.getSCPIObject(systemAccumulatorStatus));
}
