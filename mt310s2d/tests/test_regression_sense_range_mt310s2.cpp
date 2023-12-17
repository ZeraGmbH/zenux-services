#include "test_regression_sense_range_mt310s2.h"
#include "mt310s2senseinterface.h"
#include "zscpi_response_definitions.h"
#include <QTest>

QTEST_MAIN(test_regression_sense_range_mt310s2);

void test_regression_sense_range_mt310s2::init()
{
    m_scpi = new cSCPI;
    m_justData = new Mt310s2JustRangeTripletOffsetGainPhase(m_scpi); // range deletes
    m_range = new Mt310s2SenseRange(m_scpi,
                              "250V",
                              "250AliasV",
                              true,
                              1.1,
                              2.2,
                              3.3,
                              4.4,
                              5,
                              SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct,
                              m_justData);
    m_range->initSCPIConnection("SENSE:m0");
}

void test_regression_sense_range_mt310s2::cleanup()
{
    delete m_range;
    delete m_scpi;
}

void test_regression_sense_range_mt310s2::checkName()
{
    QCOMPARE(m_range->getName(), "250V");
}

void test_regression_sense_range_mt310s2::checkAlias()
{
    QString scpiAliasQuery = "SENSE:m0:250V:ALIAS?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiAliasQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAliasQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "250AliasV");

    QString scpiAliasCmd = "SENSE:m0:250V:ALIAS FOO;";
    scpiObject = m_scpi->getSCPIObject(scpiAliasCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAliasCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_mt310s2::checkAvail()
{
    QCOMPARE(m_range->getAvail(), true);
    m_range->setAvail(false);
    QCOMPARE(m_range->getAvail(), false);
    m_range->setAvail(true);
    QCOMPARE(m_range->getAvail(), true);

    QString scpiAvailQuery = "SENSE:m0:250V:AVAIL?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiAvailQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAvailQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "1");

    QString scpiAvailCmd = "SENSE:m0:250V:AVAIL 0;";
    scpiObject = m_scpi->getSCPIObject(scpiAvailCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAvailCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_mt310s2::checkUrValue()
{
    QCOMPARE(m_range->getUrvalue(), 1.1);

    QString scpiUrValueQuery = "SENSE:m0:250V:URVALUE?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiUrValueQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiUrValueQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "1.1");
}

void test_regression_sense_range_mt310s2::checkRejection()
{
    QString scpiRejectionQuery = "SENSE:m0:250V:REJECTION?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiRejectionQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "2.2");
}

void test_regression_sense_range_mt310s2::checkOvRejection()
{
    QString scpiOvRejectionQuery = "SENSE:m0:250V:OVREJECTION?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiOvRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiOvRejectionQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "3.3");
}

void test_regression_sense_range_mt310s2::checkAdcRejection()
{
    QString scpiAdcRejectionQuery = "SENSE:m0:250V:ADCREJECTION?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiAdcRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAdcRejectionQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "4.4");
}

void test_regression_sense_range_mt310s2::checkAtmelSelectionCode()
{
    QCOMPARE(m_range->getSelCode(), 5);
}

void test_regression_sense_range_mt310s2::checkTypeOrMask()
{
    quint16 expectedMask = SenseSystem::modeAC | SenseSystem::modeADJ | SenseSystem::Direct;
    QCOMPARE(m_range->getMMask(), expectedMask);

    QString scpiRejectionType = "SENSE:m0:250V:TYPE?";
    cSCPIObject* scpiObject = m_scpi->getSCPIObject(scpiRejectionType);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiRejectionType);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), QString("%1").arg(expectedMask));
}

void test_regression_sense_range_mt310s2::checkGetJustData()
{
    QCOMPARE(m_range->getJustData(), m_justData);
    // 0: Interesting but testing just data is to be done somewhere else
    QCOMPARE(m_range->getAdjustmentStatus(), 0);
}
