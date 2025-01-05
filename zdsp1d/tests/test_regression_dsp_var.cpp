#include "test_regression_dsp_var.h"
#include "dspvardevicenodeinout.h"
#include "proxy.h"
#include "zdspclient.h"
#include "zscpi_response_definitions.h"
#include "testdevicenodedsp.h"
#include "testfactorydevicenodedsp.h"
#include "testsingletondevicenodedsp.h"
#include <timemachineobject.h>
#include <tcpnetworkfactory.h>
#include <QDataStream>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_dsp_var)

static constexpr quint16 dspServerPort = 6310;

void test_regression_dsp_var::init()
{
    m_tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resmanServer = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
    m_deviceNodeFactory = std::make_shared<TestFactoryDeviceNodeDsp>();
    m_dspService = std::make_unique<TestZdsp1dForVarAccess>(m_deviceNodeFactory, m_tcpNetworkFactory);
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
    m_deviceNodeFactory = nullptr;
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

    QSignalSpy spyRead(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->dspMemoryRead(dspData);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead.count(), 1);
    QCOMPARE(spyRead[0][2], ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_dsp_var::createTempGlobalNoRead()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("createTempGlobalNoRead");
    dspData->addVarItem(new cDspVar("TempGlobal1", 1, DSPDATA::vDspTempGlobal, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("TempGlobal2", 2, DSPDATA::vDspTempGlobal, DSPDATA::dInt));

    QSignalSpy spyCreate(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyCreate.count(), 1);
    QCOMPARE(spyCreate[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    QSignalSpy spyRead(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->dspMemoryRead(dspData);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead.count(), 1);
    QCOMPARE(spyRead[0][2], ZSCPI::scpiAnswer[ZSCPI::errexec]);
}

void test_regression_dsp_var::createGlobalNoRead()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("createGlobalNoRead");
    dspData->addVarItem(new cDspVar("TempGlobal1", 1, DSPDATA::vDspTempGlobal));

    QSignalSpy spyCreate(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyCreate.count(), 1);
    QCOMPARE(spyCreate[0][2], ZSCPI::scpiAnswer[ZSCPI::ack]);

    QSignalSpy spyRead(m_dspIFace.get(), &Zera::cDSPInterface::serverAnswer);
    m_dspIFace->dspMemoryRead(dspData);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyRead.count(), 1);
    QCOMPARE(spyRead[0][2], ZSCPI::scpiAnswer[ZSCPI::errexec]);
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
    // It is for resman's sake - here it changes nothing

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

void test_regression_dsp_var::writeFloatVariablesAndListenDeviceNode()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("readVariablesAndListenDeviceNode");
    dspData->addVarItem(new cDspVar("ONE_FLOAT", 1, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("TWO_FLOAT", 2, DSPDATA::vDspResult, DSPDATA::dFloat));

    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    // Calling activateInterface is important - otherwise addresses are not initalized
    // and all variables start on address 0
    m_dspIFace->activateInterface();
    TimeMachineObject::feedEventLoop();

    dspData->setVarData("ONE_FLOAT:42.1;");
    dspData->setVarData("TWO_FLOAT:37.2,38.3;");

    TestDeviceNodeDspPtr deviceNode = TestSingletonDeviceNodeDsp::getInstancePtrTest();
    QSignalSpy spyWrite(deviceNode.get(), &TestDeviceNodeDsp::sigIoOperation);
    m_dspIFace->dspMemoryWrite(dspData);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyWrite.count(), 2);

    QCOMPARE(spyWrite[0][0], "write");
    QCOMPARE(spyWrite[0][1].toInt(), startAddress);
    QByteArray data1 = spyWrite[0][2].toByteArray();
    QCOMPARE(data1, floatToBuff(42.1));
    QCOMPARE(spyWrite[0][3], 1*varSize);

    QCOMPARE(spyWrite[1][0], "write");
    QCOMPARE(spyWrite[1][1], startAddress+1);
    QByteArray data2 = spyWrite[1][2].toByteArray();
    QCOMPARE(data2, floatToBuff(37.2) + floatToBuff(38.3));
    QCOMPARE(spyWrite[1][3], 2*varSize);
}

void test_regression_dsp_var::writeIntVariablesAndListenDeviceNode()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("readVariablesAndListenDeviceNode");
    dspData->addVarItem(new cDspVar("ONE_INT", 1, DSPDATA::vDspResult, DSPDATA::dInt));
    dspData->addVarItem(new cDspVar("TWO_INT", 2, DSPDATA::vDspResult, DSPDATA::dInt));

    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    // Calling activateInterface is important - otherwise addresses are not initalized
    // and all variables start on address 0
    m_dspIFace->activateInterface();
    TimeMachineObject::feedEventLoop();

    dspData->setVarData("ONE_INT:666;");
    dspData->setVarData("TWO_INT:37,999;");

    TestDeviceNodeDspPtr deviceNode = TestSingletonDeviceNodeDsp::getInstancePtrTest();
    QSignalSpy spyWrite(deviceNode.get(), &TestDeviceNodeDsp::sigIoOperation);
    m_dspIFace->dspMemoryWrite(dspData);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyWrite.count(), 2);

    QCOMPARE(spyWrite[0][0], "write");
    QCOMPARE(spyWrite[0][1], startAddress);
    QByteArray data1 = spyWrite[0][2].toByteArray();
    QCOMPARE(data1, intToBuff(666));
    QCOMPARE(spyWrite[0][3], varSize);

    QCOMPARE(spyWrite[1][0], "write");
    QCOMPARE(spyWrite[1][1], startAddress+1);
    QByteArray data2 = spyWrite[1][2].toByteArray();
    QCOMPARE(data2, intToBuff(37) + intToBuff(999));
    QCOMPARE(spyWrite[1][3], 2*varSize);
}

void test_regression_dsp_var::writeMixVariablesAndListenDeviceNode()
{
    cDspMeasData* dspData = m_dspIFace->getMemHandle("readVariablesAndListenDeviceNode");
    dspData->addVarItem(new cDspVar("ONE_FLOAT", 1, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("ONE_INT", 1, DSPDATA::vDspResult, DSPDATA::dInt));
    dspData->addVarItem(new cDspVar("TWO_FLOAT", 2, DSPDATA::vDspResult, DSPDATA::dFloat));
    dspData->addVarItem(new cDspVar("TWO_INT", 2, DSPDATA::vDspResult, DSPDATA::dInt));

    m_dspIFace->varList2Dsp();
    TimeMachineObject::feedEventLoop();
    // Calling activateInterface is important - otherwise addresses are not initalized
    // and all variables start on address 0
    m_dspIFace->activateInterface();
    TimeMachineObject::feedEventLoop();

    dspData->setVarData("ONE_FLOAT:42.1;");
    dspData->setVarData("ONE_INT:666;");
    dspData->setVarData("TWO_FLOAT:37.2,38.3;");
    dspData->setVarData("TWO_INT:37,999;");

    TestDeviceNodeDspPtr deviceNode = TestSingletonDeviceNodeDsp::getInstancePtrTest();
    QSignalSpy spyWrite(deviceNode.get(), &TestDeviceNodeDsp::sigIoOperation);
    m_dspIFace->dspMemoryWrite(dspData);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyWrite.count(), 4);

    QCOMPARE(spyWrite[0][0], "write");
    QCOMPARE(spyWrite[0][1].toInt(), startAddress);
    QByteArray data1 = spyWrite[0][2].toByteArray();
    QCOMPARE(data1, floatToBuff(42.1));
    QCOMPARE(spyWrite[0][3], 1*varSize);

    QCOMPARE(spyWrite[1][0], "write");
    QCOMPARE(spyWrite[1][1], startAddress+1);
    QByteArray data2 = spyWrite[1][2].toByteArray();
    QCOMPARE(data2, intToBuff(666));
    QCOMPARE(spyWrite[1][3], varSize);

    QCOMPARE(spyWrite[2][0], "write");
    QCOMPARE(spyWrite[2][1], startAddress+1+1);
    QByteArray data3 = spyWrite[2][2].toByteArray();
    QCOMPARE(data3, floatToBuff(37.2) + floatToBuff(38.3));
    QCOMPARE(spyWrite[2][3], 2*varSize);

    QCOMPARE(spyWrite[3][0], "write");
    QCOMPARE(spyWrite[3][1], startAddress+1+1+2);
    QByteArray data4 = spyWrite[3][2].toByteArray();
    QCOMPARE(data4, intToBuff(37) + intToBuff(999));
    QCOMPARE(spyWrite[3][3], 2*varSize);
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

extern TMemSection dm32DspWorkspace;

void test_regression_dsp_var::serverReadDspWorkspaceVariableAndListenDeviceNode()
{
    cZDSP1Client* testServerlient = m_dspService->createTestClient();
    TestDeviceNodeDspPtr deviceNode = TestSingletonDeviceNodeDsp::getInstancePtrTest();
    QSignalSpy spyRead(deviceNode.get(), &TestDeviceNodeDsp::sigIoOperation);

    DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);
    dspInOut.readDspVarList("FREQENCY,1;", &testServerlient->m_dspVarResolver);

    QCOMPARE(spyRead.count(), 2);
    QCOMPARE(spyRead[0][0], "lseek");
    QCOMPARE(spyRead[0][1].toInt(), dm32DspWorkspace.StartAdr);
    QCOMPARE(spyRead[1][0], "read");
    QCOMPARE(spyRead[1][1], "buf");
    QCOMPARE(spyRead[1][2], 1*varSize);
}

extern TMemSection dm32DialogWorkSpace;

void test_regression_dsp_var::serverWriteDspDialogWorkspaceVariableAndListenDeviceNode()
{
    cZDSP1Client* testServerlient = m_dspService->createTestClient();
    TestDeviceNodeDspPtr deviceNode = TestSingletonDeviceNodeDsp::getInstancePtrTest();
    QSignalSpy spyWrite(deviceNode.get(), &TestDeviceNodeDsp::sigIoOperation);

    // Note: A real world DSP write is more complex - see ZDspServer::sendCommand2Dsp
    DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);
    dspInOut.writeDspVars("DSPCMDPAR,2,42;", &testServerlient->m_dspVarResolver);

    QCOMPARE(spyWrite.count(), 1);
    QCOMPARE(spyWrite[0][0], "write");
    QCOMPARE(spyWrite[0][1].toInt(), dm32DialogWorkSpace.StartAdr);
    QCOMPARE(spyWrite[0][2], intToBuff(2) + intToBuff(42));
    QCOMPARE(spyWrite[0][3], 2*varSize);
}

QByteArray test_regression_dsp_var::floatToBuff(float value)
{
    QByteArray buffer(4, 0);
    QDataStream bas (&buffer, QIODevice::Unbuffered | QIODevice::ReadWrite);
    bas.setByteOrder(QDataStream::LittleEndian);
    bas.setFloatingPointPrecision(QDataStream::SinglePrecision);
    bas << value;
    return buffer;
}

QByteArray test_regression_dsp_var::intToBuff(qint32 value)
{
    QByteArray buffer(4, 0);
    QDataStream bas (&buffer, QIODevice::Unbuffered | QIODevice::ReadWrite);
    bas.setByteOrder(QDataStream::LittleEndian);
    bas.setFloatingPointPrecision(QDataStream::SinglePrecision);
    bas << value;
    return buffer;
}
