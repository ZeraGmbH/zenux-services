#include "test_accumulatorinterface.h"
#include <QTest>

QTEST_MAIN(test_accumulatorinterface);

static const char *systemAccumulatorStatus ="SYSTEM:ACCUMULATOR:STATUS?";
static const char *systemAccumulatorSoc ="SYSTEM:ACCUMULATOR:SOC?";

void test_accumulatorinterface::init()
{
    m_scpiInterface = std::make_unique<cSCPI>("");
    m_atmelSysCntrl = new AtmelSysCntrlTest("", 0, 0);
    m_accumulator = new AccumulatorInterface(m_scpiInterface.get(), m_atmelSysCntrl);
    m_accumulator->initSCPIConnection("");
}

void test_accumulatorinterface::cleanup()
{
    delete m_accumulator;
    delete m_atmelSysCntrl;
}

void test_accumulatorinterface::findObjects()
{
    QVERIFY(m_scpiInterface->getSCPIObject(systemAccumulatorStatus));
    QVERIFY(m_scpiInterface->getSCPIObject(systemAccumulatorSoc));
}

void test_accumulatorinterface::readAccumulatorStatus()
{
    std::unique_ptr<cProtonetCommand> protoCmd = std::make_unique<cProtonetCommand>(nullptr, false, false, QByteArray(), 0, systemAccumulatorStatus);
    cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(systemAccumulatorStatus);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd.get());
    QCOMPARE(protoCmd->m_sOutput, "0");
}

void test_accumulatorinterface::readAccumulatorSoc()
{
    std::unique_ptr<cProtonetCommand> protoCmd = std::make_unique<cProtonetCommand>(nullptr, false, false, QByteArray(), 0, systemAccumulatorSoc);
    cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(systemAccumulatorSoc);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd.get());
    QCOMPARE(protoCmd->m_sOutput, "37");
}
