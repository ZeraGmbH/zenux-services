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
    cDspMeasData* m_pTmpDataDsp = m_dspIFace->getMemHandle("foo");
    m_pTmpDataDsp->addVarItem(new cDspVar("Result1", 1, DSPDATA::vDspResult));
    m_pTmpDataDsp->addVarItem(new cDspVar("Result2", 3, DSPDATA::vDspResult));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_proxyClient);
    QCOMPARE(ret, "Result1,1;Result2,3;");
}

void test_regression_dsp_var::createTempVariables()
{
    cDspMeasData* m_pTmpDataDsp = m_dspIFace->getMemHandle("foo");
    m_pTmpDataDsp->addVarItem(new cDspVar("Temp1", 1, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new cDspVar("Temp2", 3, DSPDATA::vDspTemp));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_proxyClient);
    QCOMPARE(ret, "Temp1,1;Temp2,3;");
}

void test_regression_dsp_var::createInternalVariables()
{
    cDspMeasData* m_pTmpDataDsp = m_dspIFace->getMemHandle("foo");
    m_pTmpDataDsp->addVarItem(new cDspVar("Internal1", 1, DSPDATA::vDspIntVar));
    m_pTmpDataDsp->addVarItem(new cDspVar("Internal2", 3, DSPDATA::vDspIntVar));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_proxyClient);
    QCOMPARE(ret, ",0;"); // Is this a bug???
}

void test_regression_dsp_var::createTempGlobalVariables()
{
    cDspMeasData* m_pTmpDataDsp = m_dspIFace->getMemHandle("foo");
    m_pTmpDataDsp->addVarItem(new cDspVar("TempGlobal1", 1, DSPDATA::vDspTempGlobal));
    m_pTmpDataDsp->addVarItem(new cDspVar("TempGlobal2", 2, DSPDATA::vDspTempGlobal));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_proxyClient);
    QCOMPARE(ret, "TempGlobal1,1;TempGlobal2,2;");
}

void test_regression_dsp_var::globalVariablesAreNotSharedByDefault()
{
    cDspMeasData* m_pTmpDataDsp = m_dspIFace->getMemHandle("foo");
    m_pTmpDataDsp->addVarItem(new cDspVar("TempGlobal1", 1, DSPDATA::vDspTempGlobal));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    // not by dsp interface = different client
    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort);
    QCOMPARE(ret, "Empty");
}
