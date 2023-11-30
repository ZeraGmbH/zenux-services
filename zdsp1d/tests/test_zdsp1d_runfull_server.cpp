#include "test_zdsp1d_runfull_server.h"
#include "zdspserver.h"
#include "mockpcbserver.h"
#include "dspdevicenode.h"
#include <QAbstractEventDispatcher>
#include <QTest>

QTEST_MAIN(test_zdsp1d_runfull_server)


void test_zdsp1d_runfull_server::startServer()
{
    DspDeviceNodeInterfaceUPtr dspDeviceNode = std::make_unique<DspDeviceNode>();
    ZDspServer server(std::move(dspDeviceNode), MockPcbServer::createParams("zdsp1d"));
    feedEventLoop();
}

void test_zdsp1d_runfull_server::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
