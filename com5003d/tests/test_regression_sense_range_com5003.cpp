#include "test_regression_sense_range_com5003.h"
#include "com5003sensechannel.h"
#include "senseinterfacecommon.h"
#include "zscpi_response_definitions.h"
#include <QTest>

QTEST_MAIN(test_regression_sense_range_com5003);

void test_regression_sense_range_com5003::init()
{
    m_scpiInterface = std::make_shared<cSCPI>();
    m_range = new SenseRangeCommon(m_scpiInterface,
                                   "240V",
                                   true,
                                   11111.11111,
                                   22222.22222,
                                   33333.33333,
                                   5,
                                   modeAC,
                                   new AdjRangeScpi(m_scpiInterface, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter()),
                                   rejectionScpiQueryDigitsCom5003);
    m_range->initSCPIConnection("SENSE:m0");
}

void test_regression_sense_range_com5003::cleanup()
{
    delete m_range;
}

void test_regression_sense_range_com5003::checkName()
{
    QCOMPARE(m_range->getRangeName(), "240V");
}

void test_regression_sense_range_com5003::checkAlias()
{
    QString scpiAliasQuery = "SENSE:m0:240V:ALIAS?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiAliasQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAliasQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "240V");

    QString scpiAliasCmd = "SENSE:m0:240V:ALIAS FOO;";
    scpiObject = m_scpiInterface->getSCPIObject(scpiAliasCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAliasCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_com5003::checkAvail()
{
    QCOMPARE(m_range->getAvail(), true);
    QString scpiAvailQuery = "SENSE:m0:240V:AVAIL?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiAvailQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAvailQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "1");

    QString scpiAvailCmd = "SENSE:m0:240V:AVAIL 0;";
    scpiObject = m_scpiInterface->getSCPIObject(scpiAvailCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAvailCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_com5003::checkUrValue()
{
    QCOMPARE(m_range->getUpperRangevalue(), 11111.11111);

    QString scpiUrValueQuery = "SENSE:m0:240V:URVALUE?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiUrValueQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiUrValueQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "11111.1");

    QString scpiUrValueCmd = "SENSE:m0:240V:URVALUE 42";
    scpiObject = m_scpiInterface->getSCPIObject(scpiUrValueCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiUrValueCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_com5003::checkRejection()
{
    QString scpiRejectionQuery = "SENSE:m0:240V:REJECTION?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiRejectionQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "22222.2");

    QString scpiRejectionCmd = "SENSE:m0:240V:REJECTION 42";
    scpiObject = m_scpiInterface->getSCPIObject(scpiRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiRejectionCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_com5003::checkOvRejection()
{
    QString scpiOvRejectionQuery = "SENSE:m0:240V:OVREJECTION?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiOvRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiOvRejectionQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "33333.3");

    QString scpiOvRejectionCmd = "SENSE:m0:240V:OVREJECTION 42";
    scpiObject = m_scpiInterface->getSCPIObject(scpiOvRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiOvRejectionCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_com5003::checkAdcRejection()
{
    QString scpiAdcRejectionQuery = "SENSE:m0:240V:ADCREJECTION?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiAdcRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAdcRejectionQuery);
    ScpiDelegate *scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "8388607");

    QString scpiAdcRejectionCmd = "SENSE:m0:240V:ADCREJECTION 42";
    scpiObject = m_scpiInterface->getSCPIObject(scpiAdcRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiAdcRejectionCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_sense_range_com5003::checkAtmelSelectionCode()
{
    QCOMPARE(m_range->getSelCode(), 5);
}

void test_regression_sense_range_com5003::checkTypeOrMask()
{
    QString scpiRejectionType = "SENSE:m0:240V:TYPE?";
    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiRejectionType);
    QVERIFY(scpiObject != nullptr);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiRejectionType);
    ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), QString("%1").arg(modeAC));

    QString scpiRejectionCmd = "SENSE:m0:240V:TYPE 1";
    scpiObject = m_scpiInterface->getSCPIObject(scpiRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, QByteArray(), 0, scpiRejectionCmd);
    scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput),ZSCPI::scpiAnswer[ZSCPI::nak]);
}
