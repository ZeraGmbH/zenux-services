#include "test_zdsp1d_runfull_server.h"
#include "zdspserver.h"
#include "mockpcbserver.h"
#include "dspdevicenode.h"
#include "dspdevicenodemock.h"
#include <QAbstractEventDispatcher>
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
    DspDeviceNodeInterfaceUPtr dspDeviceNode = std::make_unique<DspDeviceNodeMock>(DspDeviceNode::MAGIC_ID21362);
    ZDspServer server(std::move(dspDeviceNode), MockPcbServer::createParams("zdsp1d"));
    feedEventLoop();
}

void test_zdsp1d_runfull_server::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
