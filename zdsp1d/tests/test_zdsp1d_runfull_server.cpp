#include "test_zdsp1d_runfull_server.h"
#include "zdspserver.h"
#include "mockpcbserver.h"
#include "dspdevicenode.h"
#include "dspdevicenodemock.h"
#include "resmanrunfacade.h"
#include "proxy.h"
#include "dspinterface.h"
#include "reply.h"
#include <QAbstractEventDispatcher>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_zdsp1d_runfull_server)


void test_zdsp1d_runfull_server::startProductionServerToSeeFailingMessages()
{
    DspDeviceNodeInterfaceUPtr dspDeviceNode = std::make_unique<DspDeviceNode>();
    ZDspServer server(std::move(dspDeviceNode), MockPcbServer::createParams("zdsp1d"));
    feedEventLoop();
}

void test_zdsp1d_runfull_server::startMockedServer()
{
    ResmanRunFacade resman;
    feedEventLoop();

    DspDeviceNodeInterfaceUPtr dspDeviceNode = std::make_unique<DspDeviceNodeMock>(DspDeviceNode::MAGIC_ID21362);
    ZDspServer server(std::move(dspDeviceNode), MockPcbServer::createParams("zdsp1d"));
    feedEventLoop();

    Zera::ProxyClientPtr dspClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6310);
    Zera::cDSPInterface dspIFace;
    dspIFace.setClientSmart(dspClient);

    Zera::Proxy::getInstance()->startConnectionSmart(dspClient);
    feedEventLoop();

    QSignalSpy responseSpy(&dspIFace, &Zera::cDSPInterface::serverAnswer);
    int msgNr = dspIFace.readDeviceVersion();
    feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("DSPLCA: V0.0;DSP V0.00"));
}

void test_zdsp1d_runfull_server::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
