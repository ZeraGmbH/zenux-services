#include "test_zdsp1d_runfull_server.h"
#include "zdspserver.h"
#include "mockpcbserver.h"
#include <QAbstractEventDispatcher>
#include <QTest>

QTEST_MAIN(test_zdsp1d_runfull_server)


void test_zdsp1d_runfull_server::startServer()
{
    ZDspServer server(MockPcbServer::createParams("zdsp1d"));
    feedEventLoop();
}

void test_zdsp1d_runfull_server::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
