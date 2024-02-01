#include "test_regression_dsp_var.h"
#include "proxy.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_dsp_var)

void test_regression_dsp_var::init()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_dspService = std::make_unique<MockZdsp1d>();
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6310);
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
    m_pTmpDataDsp->addVarItem(new cDspVar("Result2", 2, DSPDATA::vDspResult));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_regression_dsp_var::createTempVariables()
{
    cDspMeasData* m_pTmpDataDsp = m_dspIFace->getMemHandle("foo");
    m_pTmpDataDsp->addVarItem(new cDspVar("Temp1", 1, DSPDATA::vDspTemp));
    m_pTmpDataDsp->addVarItem(new cDspVar("Temp2", 2, DSPDATA::vDspTemp));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_regression_dsp_var::createInternalVariables()
{
    cDspMeasData* m_pTmpDataDsp = m_dspIFace->getMemHandle("foo");
    m_pTmpDataDsp->addVarItem(new cDspVar("Internal1", 1, DSPDATA::vDspIntVar));
    m_pTmpDataDsp->addVarItem(new cDspVar("Internal2", 2, DSPDATA::vDspIntVar));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}

void test_regression_dsp_var::createTempGlobalVariables()
{
    cDspMeasData* m_pTmpDataDsp = m_dspIFace->getMemHandle("foo");
    m_pTmpDataDsp->addVarItem(new cDspVar("TempGlobal1", 1, DSPDATA::vDspIntVar));
    m_pTmpDataDsp->addVarItem(new cDspVar("TempGlobal2", 2, DSPDATA::vDspIntVar));

    QSignalSpy spy(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 1);
}
