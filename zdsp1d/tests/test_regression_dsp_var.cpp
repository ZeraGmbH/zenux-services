#include "test_regression_dsp_var.h"
#include "proxy.h"
#include <zscpi_response_definitions.h>
#include "scpisingletransactionblocked.h"
#include "testdevicenodedsp.h"
#include "testfactorydevicenodedsp.h"
#include "testsingletondevicenodedsp.h"
#include "dspinterfacecmddecoder.h"
#include <timemachineobject.h>
#include <tcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_dsp_var)

static constexpr quint16 dspServerPort = 6310;

void test_regression_dsp_var::init()
{
    m_tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
    m_dspService = std::make_unique<MockZdsp1d>(std::make_shared<TestFactoryDeviceNodeDsp>(), m_tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", dspServerPort, m_tcpNetworkFactory);
    m_dspIFace = std::make_unique<Zera::cDSPInterface>();
    m_dspIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_dsp_var::cleanup()
{
    m_dspIFace = nullptr;
    TimeMachineObject::feedEventLoop();
    m_proxyClient = nullptr;
    TimeMachineObject::feedEventLoop();

    m_dspService = nullptr;
    TimeMachineObject::feedEventLoop();
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

    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_tcpNetworkFactory, m_proxyClient);
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

    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_tcpNetworkFactory, m_proxyClient);
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
    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_tcpNetworkFactory, m_proxyClient);
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
    QString ret = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_tcpNetworkFactory, m_proxyClient);
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

static constexpr int dm32UserWorkSpaceBase21362 = 0x98180; // Stolen from zdspserver.cpp !!!
static constexpr int startAddress = dm32UserWorkSpaceBase21362;
static constexpr int varSize = 4;

void test_regression_dsp_var::readVariablesAndListenDeviceNode()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("readVariablesAndListenDeviceNode");
    dspData->addVarItem(new cDspVar("ONE_FLOAT", 1, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("TWO_FLOAT", 2, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("THREE_INT", 3, DSPDATA::vDspResult, DSPDATA::dInt));

    // In modules we have claimUSERMem() - we had this here during investigation
    // It is for resman's sake - here is changes nothing

    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    // Calling activateInterface is important - otherwise addresses are not initalized
    // and all variables start on address 0
    m_dspIFace->activateInterface();
    TimeMachineObject::feedEventLoop();

    TestDeviceNodeDspPtr deviceNode = TestSingletonDeviceNodeDsp::getInstancePtrTest();
    QSignalSpy spyRead(deviceNode.get(), &TestDeviceNodeDsp::sigIoOperation);
    m_dspIFace->dspMemoryRead(dspData);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyRead.count(), 3*2);

    QCOMPARE(spyRead[0][0], "lseek");
    QCOMPARE(spyRead[0][1].toInt(), startAddress);
    QCOMPARE(spyRead[1][0], "read");
    QCOMPARE(spyRead[1][1], "buf");
    QCOMPARE(spyRead[1][2], 1*varSize);

    QCOMPARE(spyRead[2][0], "lseek");
    QCOMPARE(spyRead[2][1], startAddress+1);
    QCOMPARE(spyRead[3][0], "read");
    QCOMPARE(spyRead[3][1], "buf");
    QCOMPARE(spyRead[3][2], 2*varSize);

    QCOMPARE(spyRead[4][0], "lseek");
    QCOMPARE(spyRead[4][1], startAddress+1+2);
    QCOMPARE(spyRead[5][0], "read");
    QCOMPARE(spyRead[5][1], "buf");
    QCOMPARE(spyRead[5][2], 3*varSize);
}

void test_regression_dsp_var::writeVariablesAndListenDeviceNode()
{
    // Important note: With code at the time of wtiting sending multiple integers
    // fails
    cDspMeasData* dspData = m_dspIFace->getMemHandle("readVariablesAndListenDeviceNode");
    dspData->addVarItem(new cDspVar("ONE_FLOAT", 1, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("TWO_FLOAT", 2, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("ONE_INT", 1, DSPDATA::vDspResult, DSPDATA::dInt));

    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    // Calling activateInterface is important - otherwise addresses are not initalized
    // and all variables start on address 0
    m_dspIFace->activateInterface();
    TimeMachineObject::feedEventLoop();

    DspInterfaceCmdDecoder::setVarData(dspData, QString("ONE_FLOAT:42;"), DSPDATA::dInt);
    DspInterfaceCmdDecoder::setVarData(dspData, QString("TWO_FLOAT:37,38;"), DSPDATA::dInt);
    DspInterfaceCmdDecoder::setVarData(dspData, QString("ONE_INT:666;"), DSPDATA::dInt);

    TestDeviceNodeDspPtr deviceNode = TestSingletonDeviceNodeDsp::getInstancePtrTest();
    QSignalSpy spyWrite(deviceNode.get(), &TestDeviceNodeDsp::sigIoOperation);
    m_dspIFace->dspMemoryWrite(dspData);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyWrite.count(), 3);

    QCOMPARE(spyWrite[0][0], "write");
    QCOMPARE(spyWrite[0][1].toInt(), startAddress);
    QByteArray data1 = spyWrite[0][2].toByteArray();
    QCOMPARE(quint8(data1[0]), 42);
    QCOMPARE(spyWrite[0][3], 1*varSize);

    QCOMPARE(spyWrite[1][0], "write");
    QCOMPARE(spyWrite[1][1], startAddress+1);
    QByteArray data2 = spyWrite[1][2].toByteArray();
    QCOMPARE(quint8(data2[varSize*0]), 37);
    QCOMPARE(quint8(data2[varSize*1]), 38);
    QCOMPARE(spyWrite[1][3], 2*varSize);

    // TODO: What exactly are the contents we see here?
    QCOMPARE(spyWrite[2][0], "write");
    QCOMPARE(spyWrite[2][1], startAddress+1+2);
    QByteArray data3 = spyWrite[2][2].toByteArray();
    QCOMPARE(quint8(data3[varSize*0]), 154);
    QCOMPARE(spyWrite[2][3], varSize);
}

void test_regression_dsp_var::multipleClientsCreateResultVars()
{
    // create vars client1
    cDspMeasData* dspData1 = m_dspIFace->getMemHandle("createResultVariables");
    dspData1->addVarItem(new cDspVar("CLIENT1_VAR1", 1, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData1->addVarItem(new cDspVar("CLIENT1_VAR2", 2, DSPDATA::vDspResult, DSPDATA::dFloat));
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    // connect client2
    Zera::ProxyClientPtr proxyClient2 = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", dspServerPort, m_tcpNetworkFactory);
    std::unique_ptr<Zera::cDSPInterface> dspIFace2 = std::make_unique<Zera::cDSPInterface>();
    dspIFace2->setClientSmart(proxyClient2);
    Zera::Proxy::getInstance()->startConnectionSmart(proxyClient2);
    TimeMachineObject::feedEventLoop();

    // create vars client2
    cDspMeasData* dspData2 = dspIFace2->getMemHandle("createResultVariables");
    dspData2->addVarItem(new cDspVar("CLIENT2_VAR1", 3, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData2->addVarItem(new cDspVar("CLIENT2_VAR2", 4, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspIFace2->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    // client1 query vars
    QString ret1 = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_tcpNetworkFactory, m_proxyClient);
    QCOMPARE(ret1, "CLIENT1_VAR1,1;CLIENT1_VAR2,2;");
    // client1 read client1 vars
    QSignalSpy spyRead1(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->dspMemoryRead(dspData1);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead1[0][1], ZSCPI::ack);
    QVERIFY(spyRead1[0][2].toString().contains("CLIENT1_VAR1:"));
    QVERIFY(spyRead1[0][2].toString().contains("CLIENT1_VAR2:"));
    spyRead1.clear();
    // client1 read client2 vars
    m_dspIFace->dspMemoryRead(dspData2);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead1[0][2].toString(), "errexec");

    // client2 query vars
    QString ret2 = ScpiSingleTransactionBlocked::query("MEASURE:LIST:RAVLIST?", dspServerPort, m_tcpNetworkFactory, proxyClient2);
    QCOMPARE(ret2, "CLIENT2_VAR1,3;CLIENT2_VAR2,4;");
    // client2 read client2 vars
    QSignalSpy spyRead2(dspIFace2.get(), &Zera::cDSPInterface::serverAnswer);
    dspIFace2->dspMemoryRead(dspData2);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead2[0][1], ZSCPI::ack);
    QVERIFY(spyRead2[0][2].toString().contains("CLIENT2_VAR1:"));
    QVERIFY(spyRead2[0][2].toString().contains("CLIENT2_VAR2:"));
    spyRead2.clear();
    // client2 read client1 vars
    dspIFace2->dspMemoryRead(dspData1);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead2[0][2].toString(), "errexec");
}
