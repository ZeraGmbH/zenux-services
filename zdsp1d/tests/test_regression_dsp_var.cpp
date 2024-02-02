#include "test_regression_dsp_var.h"
#include "proxy.h"
#include <zscpi_response_definitions.h>
#include "scpisingletransactionblocked.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_dsp_var)

static constexpr quint16 dspServerPort = 6310;

void test_regression_dsp_var::init()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_dspService = std::make_unique<MockZdsp1d>();
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", dspServerPort);
    m_dspIFace = std::make_unique<Zera::cDSPInterface>();
    m_dspIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_dsp_var::cleanup()
{
    m_dspIFace = nullptr;
    m_proxyClient = nullptr;
    m_dspService = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_dsp_var::createResultVariables()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("createResultVariables");
    dspData->addVarItem(new cDspVar("Result1", 1, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("Result2", 3, DSPDATA::vDspResult, DSPDATA::dInt));

    QSignalSpy spyCreate(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyCreate.count(), 1);
    QCOMPARE(spyCreate[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_proxyClient);
    QCOMPARE(ret, "RESULT1,1;RESULT2,3;");

    QSignalSpy spyRead(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->dspMemoryRead(dspData);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead.count(), 1);
    QCOMPARE(spyRead[0][1], ZSCPI::ack);
    QVERIFY(spyRead[0][2].toString().contains("RESULT1:"));
    QVERIFY(spyRead[0][2].toString().contains("RESULT2:"));
}

void test_regression_dsp_var::createTempVariables()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("createTempVariables");
    dspData->addVarItem(new cDspVar("Temp1", 1, DSPDATA::vDspTemp, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("Temp2", 3, DSPDATA::vDspTemp, DSPDATA::dInt));

    QSignalSpy spyCreate(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyCreate.count(), 1);
    QCOMPARE(spyCreate[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_proxyClient);
    QCOMPARE(ret, "TEMP1,1;TEMP2,3;");

    QSignalSpy spyRead(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->dspMemoryRead(dspData);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead.count(), 1);
    QCOMPARE(spyRead[0][1], ZSCPI::ack);
    QVERIFY(spyRead[0][2].toString().contains("TEMP1:"));
    QVERIFY(spyRead[0][2].toString().contains("TEMP2:"));
}

void test_regression_dsp_var::createInternalVariables()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("createInternalVariables");
    dspData->addVarItem(new cDspVar("Internal1", 1, DSPDATA::vDspIntVar, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("Internal2", 3, DSPDATA::vDspIntVar, DSPDATA::dInt));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    // We can neither list nor read but not EMPTY??
    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_proxyClient);
    QCOMPARE(ret, ",0;");

    QSignalSpy spyRead(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->dspMemoryRead(dspData);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead.count(), 1);
    QCOMPARE(spyRead[0][2], ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_dsp_var::createTempGlobalVariables()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("createTempGlobalVariables");
    dspData->addVarItem(new cDspVar("TempGlobal1", 1, DSPDATA::vDspTempGlobal, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("TempGlobal2", 2, DSPDATA::vDspTempGlobal, DSPDATA::dInt));

    QSignalSpy spyCreate(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyCreate.count(), 1);
    QCOMPARE(spyCreate[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    // We can list but not read
    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_proxyClient);
    QCOMPARE(ret, "TEMPGLOBAL1,1;TEMPGLOBAL2,2;");

    QSignalSpy spyRead(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->dspMemoryRead(dspData);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead.count(), 1);
    QCOMPARE(spyRead[0][2], ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_dsp_var::globalVariablesAreNotSharedByDefault()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("globalVariablesAreNotSharedByDefault");
    dspData->addVarItem(new cDspVar("TempGlobal1", 1, DSPDATA::vDspTempGlobal));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    // not by dsp interface = different client
    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort);
    QCOMPARE(ret, "Empty");
}
