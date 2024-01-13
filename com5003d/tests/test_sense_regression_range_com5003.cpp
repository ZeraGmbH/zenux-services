#include "test_sense_regression_range_com5003.h"
#include "zscpi_response_definitions.h"
#include <QTest>

QTEST_MAIN(test_sense_regression_range_com5003);

void test_sense_regression_range_com5003::init()
{
    m_scpi = new cSCPI;
    m_range = new Com5003SenseRange(m_scpi,
                                    "240V",
                                    true,
                                    11111.11111,
                                    22222.22222,
                                    33333.33333,
                                    5);
    m_range->initSCPIConnection("SENSE:m0");
}

void test_sense_regression_range_com5003::cleanup()
{
    delete m_range;
    delete m_scpi;
}

void test_sense_regression_range_com5003::checkName()
{
    QCOMPARE(m_range->getName(), "240V");
}

void test_sense_regression_range_com5003::checkAlias()
{
    QString scpiAliasQuery = "SENSE:m0:240V:ALIAS?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiAliasQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAliasQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "240V");

    QString scpiAliasCmd = "SENSE:m0:240V:ALIAS FOO;";
    scpiObject = m_scpi->getSCPIObject(scpiAliasCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAliasCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_sense_regression_range_com5003::checkAvail()
{
    QCOMPARE(m_range->getAvail(), true);
    m_range->setAvail(false);
    QCOMPARE(m_range->getAvail(), false);

    QString scpiAvailQuery = "SENSE:m0:240V:AVAIL?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiAvailQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAvailQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "0");

    m_range->setAvail(true);
    QCOMPARE(m_range->getAvail(), true);
    scpiObject = m_scpi->getSCPIObject(scpiAvailQuery);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAvailQuery);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "1");

    QString scpiAvailCmd = "SENSE:m0:240V:AVAIL 0;";
    scpiObject = m_scpi->getSCPIObject(scpiAvailCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAvailCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_sense_regression_range_com5003::checkUrValue()
{
    QCOMPARE(m_range->getUpperRangevalue(), 11111.11111);

    QString scpiUrValueQuery = "SENSE:m0:240V:URVALUE?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiUrValueQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiUrValueQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "11111.1");

    QString scpiUrValueCmd = "SENSE:m0:240V:URVALUE 42";
    scpiObject = m_scpi->getSCPIObject(scpiUrValueCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiUrValueCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_sense_regression_range_com5003::checkRejection()
{
    QString scpiRejectionQuery = "SENSE:m0:240V:REJECTION?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiRejectionQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "22222.2");

    QString scpiRejectionCmd = "SENSE:m0:240V:REJECTION 42";
    scpiObject = m_scpi->getSCPIObject(scpiRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiRejectionCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_sense_regression_range_com5003::checkOvRejection()
{
    QString scpiOvRejectionQuery = "SENSE:m0:240V:OVREJECTION?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiOvRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiOvRejectionQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "33333.3");

    QString scpiOvRejectionCmd = "SENSE:m0:240V:OVREJECTION 42";
    scpiObject = m_scpi->getSCPIObject(scpiOvRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiOvRejectionCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_sense_regression_range_com5003::checkAdcRejection()
{
    QString scpiAdcRejectionQuery = "SENSE:m0:240V:ADCREJECTION?";
    cSCPIObject *scpiObject = m_scpi->getSCPIObject(scpiAdcRejectionQuery);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand *protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAdcRejectionQuery);
    cSCPIDelegate *scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), "8.38861e+06"); // we should increase accurracy...

    QString scpiAdcRejectionCmd = "SENSE:m0:240V:ADCREJECTION 42";
    scpiObject = m_scpi->getSCPIObject(scpiAdcRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiAdcRejectionCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_sense_regression_range_com5003::checkAtmelSelectionCode()
{
    QCOMPARE(m_range->getSelCode(), 5);
}

void test_sense_regression_range_com5003::checkTypeOrMask()
{
    QString scpiRejectionType = "SENSE:m0:240V:TYPE?";
    cSCPIObject* scpiObject = m_scpi->getSCPIObject(scpiRejectionType);
    QVERIFY(scpiObject != nullptr);
    cProtonetCommand* protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiRejectionType);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput), QString("%1").arg(0)); // was SenseRange::Phys - all ranges have that

    QString scpiRejectionCmd = "SENSE:m0:240V:TYPE 1";
    scpiObject = m_scpi->getSCPIObject(scpiRejectionCmd);
    QVERIFY(scpiObject != nullptr);
    protoCmd = new cProtonetCommand(0, false, true, QByteArray(), 0, scpiRejectionCmd);
    scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
    QCOMPARE((protoCmd->m_sOutput),ZSCPI::scpiAnswer[ZSCPI::nak]);
}
