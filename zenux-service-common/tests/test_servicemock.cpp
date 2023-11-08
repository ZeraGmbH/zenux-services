#include "test_servicemock.h"
#include "mockmt310s2d.h"
#include "proxy.h"
#include "pcbinterface.h"
#include <QAbstractEventDispatcher>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_servicemock)

void test_servicemock::init()
{
    m_resMan = std::make_shared<ResmanRunFacade>();
    feedEventLoop();
}

void test_servicemock::runMockMt310s2d()
{
    MockMt310s2d mtMockService;
    mtMockService.start();
    feedEventLoop();
    QVERIFY(mtMockService.areAllResourcesConnected());
}

void test_servicemock::connectMt310s2d()
{
    MockMt310s2d mtMockService;
    mtMockService.start();
    feedEventLoop();

    Zera::ProxyClient* pcbClient = Zera::Proxy::getInstance()->getConnection("127.0.0.1", 6308);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClient(pcbClient);
    QSignalSpy connectSpy(pcbClient, &Zera::ProxyClient::connected);
    Zera::Proxy::getInstance()->startConnection(pcbClient);
    feedEventLoop();
    QCOMPARE(connectSpy.count(), 1);
}

void test_servicemock::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
