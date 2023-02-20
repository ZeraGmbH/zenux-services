#include "test_accumulatorinterface.h"
#include <QTest>

QTEST_MAIN(test_accumulatorinterface);

static const char *systemAccumulatorStatus ="SYSTEM:ACCUMULATOR:STATUS?";

void test_accumulatorinterface::init()
{
    m_scpiInterface = std::make_unique<cSCPI>("");
    m_accumulator = new AccumulatorInterface(m_scpiInterface.get());
    m_accumulator->initSCPIConnection("");
}

void test_accumulatorinterface::cleanup()
{
    delete m_accumulator;
}

void test_accumulatorinterface::findObject()
{
    QVERIFY(m_scpiInterface->getSCPIObject(systemAccumulatorStatus));
}

void test_accumulatorinterface::readAccumulatorStatus()
{
    std::unique_ptr<cProtonetCommand> protoCmd = std::make_unique<cProtonetCommand>(nullptr, false, false, QByteArray(), 0, systemAccumulatorStatus);
    cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(systemAccumulatorStatus);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    //scpiDelegate->executeSCPI(protoCmd.get());
    //QCOMPARE(protoCmd->m_sOutput, "0");
}
