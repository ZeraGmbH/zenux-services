#include "test_mockservice_mt310s2d.h"
#include "mockmt310s2d.h"
#include "resmanrunfacade.h"
#include "pcbinterface.h"
#include "proxy.h"
#include "reply.h"
#include <QAbstractEventDispatcher>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_mt310s2d)

void test_mockservice_mt310s2d::runMock()
{
    ResmanRunFacade resman;
    MockMt310s2d mockMt310s2d;
    feedEventLoop();
    QVERIFY(mockMt310s2d.areAllResourcesConnected());
}

void test_mockservice_mt310s2d::connectServer()
{
    ResmanRunFacade resman;
    MockMt310s2d mockMt310s2d;
    feedEventLoop();

    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(pcbClient);

    QSignalSpy connectSpy(pcbClient.get(), &Zera::ProxyClient::connected);
    Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    feedEventLoop();
    QCOMPARE(connectSpy.count(), 1);
}

void test_mockservice_mt310s2d::connectInvalidIp()
{
    ResmanRunFacade resman;
    MockMt310s2d mockMt310s2d;
    feedEventLoop();

    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("128.0.0.1", 6307);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(pcbClient);

    QSignalSpy connectSpy(pcbClient.get(), &Zera::ProxyClient::connected);
    Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    feedEventLoop();
    QCOMPARE(connectSpy.count(), 0);
}

void test_mockservice_mt310s2d::getFoutCat()
{
    ResmanRunFacade resman;
    MockMt310s2d mockMt310s2d;
    feedEventLoop();

    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(pcbClient);

    Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    feedEventLoop();

    QSignalSpy responseSpy(&pcbIFace, &Zera::cPCBInterface::serverAnswer);
    int msgNr = pcbIFace.getSourceCat();
    feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("fo0;fo1;fo2;fo3"));
}

void test_mockservice_mt310s2d::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
