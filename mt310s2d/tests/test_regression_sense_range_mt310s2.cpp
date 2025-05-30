#include "test_regression_sense_range_mt310s2.h"
#include "mtxxxs2sensechannel.h"
#include "senseinterfacecommon.h"
#include "zscpi_response_definitions.h"
#include <QTest>

QTEST_MAIN(test_regression_sense_range_mt310s2);

void test_regression_sense_range_mt310s2::init()
{
    m_scpiInterface = std::make_shared<cSCPI>();
    m_range = new SenseRangeCommon(m_scpiInterface,
                              "250V",
                              true,
                              11111.11111,
                              22222.22222,
                              33333.33333,
                              5,
                              modeAC | modeADJ | Direct,
                              new AdjRangeScpi(m_scpiInterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter()),
                              rejectionScpiQueryDigitsMtXXXs2);
    m_range->initSCPIConnection("SENSE:m0");
}

void test_regression_sense_range_mt310s2::cleanup()
{
    delete m_range;
}

void test_regression_sense_range_mt310s2::checkName()
{
    QCOMPARE(m_range->getRangeName(), "250V");
}

void test_regression_sense_range_mt310s2::checkAlias()
{
    QString scpiAliasQuery = "SENSE:m0:250V:ALIAS?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiAliasQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAliasQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "250V");

    QString scpiAliasCmd = "SENSE:m0:250V:ALIAS FOO;";
    scpiObject = m_scpiInterface->getSCPIObject(scpiAliasCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAliasCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_mt310s2::checkAvail()
{
    QCOMPARE(m_range->getAvail(), true);
    QString scpiAvailQuery = "SENSE:m0:250V:AVAIL?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiAvailQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAvailQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "1");

    QString scpiAvailCmd = "SENSE:m0:250V:AVAIL 0;";
    scpiObject = m_scpiInterface->getSCPIObject(scpiAvailCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAvailCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}


// Interesting: here we don't need trailing SCPI ';' !!! (debugged)
void test_regression_sense_range_mt310s2::checkUrValue()
{
    QCOMPARE(m_range->getUpperRangevalue(), 11111.11111);

    QString scpiUrValueQuery = "SENSE:m0:250V:URVALUE?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiUrValueQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiUrValueQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "11111.1");

    QString scpiUrValueCmd = "SENSE:m0:250V:URVALUE 42";
    scpiObject = m_scpiInterface->getSCPIObject(scpiUrValueCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiUrValueCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_mt310s2::checkRejection()
{
    QString scpiRejectionQuery = "SENSE:m0:250V:REJECTION?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiRejectionQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "22222.222");

    QString scpiRejectionCmd = "SENSE:m0:250V:REJECTION 42";
    scpiObject = m_scpiInterface->getSCPIObject(scpiRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiRejectionCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_mt310s2::checkOvRejection()
{
    QString scpiOvRejectionQuery = "SENSE:m0:250V:OVREJECTION?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiOvRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiOvRejectionQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "33333.333");

    QString scpiOvRejectionCmd = "SENSE:m0:250V:OVREJECTION 42";
    scpiObject = m_scpiInterface->getSCPIObject(scpiOvRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiOvRejectionCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_mt310s2::checkAdcRejection()
{
    QString scpiAdcRejectionQuery = "SENSE:m0:250V:ADCREJECTION?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiAdcRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAdcRejectionQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "8388607");

    QString scpiAdcRejectionCmd = "SENSE:m0:250V:ADCREJECTION 42";
    scpiObject = m_scpiInterface->getSCPIObject(scpiAdcRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAdcRejectionCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_mt310s2::checkAtmelSelectionCode()
{
    QCOMPARE(m_range->getSelCode(), 5);
}

void test_regression_sense_range_mt310s2::checkTypeOrMask()
{
    quint16 expectedMask = modeAC | modeADJ | Direct;
    QCOMPARE(m_range->getMMask(), expectedMask);

    QString scpiRejectionType = "SENSE:m0:250V:TYPE?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiRejectionType);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiRejectionType);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), QString("%1").arg(expectedMask));

    QString scpiRejectionCmd = "SENSE:m0:250V:TYPE 1";
    scpiObject = m_scpiInterface->getSCPIObject(scpiRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiRejectionCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput),ZSCPI::scpiAnswer[ZSCPI::nak]);
}
