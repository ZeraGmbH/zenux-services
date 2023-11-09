#include "test_servicemock.h"
#include "mockpcbserver.h"
#include "resmanrunfacade.h"
#include "foutgroupresourceandinterface.h"
#include "secgroupresourceandinterface.h"
#include "proxy.h"
#include "pcbinterface.h"
#include "secinterface.h"
#include "reply.h"
#include <QAbstractEventDispatcher>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_servicemock)

void test_servicemock::runMockMt310s2d()
{
    ResmanRunFacade resman;
    MockPcbServer mock("mt310s2d");
    FOutSettings foutSettings(mock.getConfigReader());
    mock.setXmlSettings(QList<XMLSettings*>() << &foutSettings);
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    mock.setResources(QList<cResource*>() << &foutResource);
    mock.start();
    feedEventLoop();
    QVERIFY(mock.areAllResourcesConnected());
}

void test_servicemock::connectMt310s2d()
{
    ResmanRunFacade resman;
    MockPcbServer mock("mt310s2d");
    FOutSettings foutSettings(mock.getConfigReader());
    mock.setXmlSettings(QList<XMLSettings*>() << &foutSettings);
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    mock.setResources(QList<cResource*>() << &foutResource);

    mock.start();
    feedEventLoop();

    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(pcbClient);

    QSignalSpy connectSpy(pcbClient.get(), &Zera::ProxyClient::connected);
    Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    feedEventLoop();
    QCOMPARE(connectSpy.count(), 1);
}

void test_servicemock::connectInvalidIpMt310s2d()
{
    ResmanRunFacade resman;
    MockPcbServer mock("mt310s2d");
    FOutSettings foutSettings(mock.getConfigReader());
    mock.setXmlSettings(QList<XMLSettings*>() << &foutSettings);
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    mock.setResources(QList<cResource*>() << &foutResource);

    mock.start();
    feedEventLoop();

    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("128.0.0.1", 6307);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(pcbClient);

    QSignalSpy connectSpy(pcbClient.get(), &Zera::ProxyClient::connected);
    Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    feedEventLoop();
    QCOMPARE(connectSpy.count(), 0);
}

void test_servicemock::getFoutCatMt310s2d()
{
    ResmanRunFacade resman;
    MockPcbServer mock("mt310s2d");
    FOutSettings foutSettings(mock.getConfigReader());
    mock.setXmlSettings(QList<XMLSettings*>() << &foutSettings);
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    mock.setResources(QList<cResource*>() << &foutResource);

    mock.start();
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

void test_servicemock::getFoutCatCom5003d()
{
    ResmanRunFacade resman;
    MockPcbServer mock("com5003d");
    FOutSettings foutSettings(mock.getConfigReader());
    mock.setXmlSettings(QList<XMLSettings*>() << &foutSettings);
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    mock.setResources(QList<cResource*>() << &foutResource);

    mock.start();
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

void test_servicemock::getChannelCatSec1000d()
{
    ResmanRunFacade resman;
    MockPcbServer mock("sec1000d");

    SecCalculatorSettings secSettings(mock.getConfigReader());
    SecInputSettings secInputSettings(mock.getConfigReader());
    mock.setXmlSettings(QList<XMLSettings*>() << &secSettings << &secInputSettings);
    SecGroupResourceAndInterface secInterface(-1, &secSettings, &secInputSettings, nullptr);
    mock.setResources(QList<cResource*>() << &secInterface);

    mock.start();
    feedEventLoop();

    Zera::ProxyClientPtr secClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6305);
    Zera::cSECInterface secIFace;
    secIFace.setClientSmart(secClient);

    Zera::Proxy::getInstance()->startConnectionSmart(secClient);
    feedEventLoop();

    QSignalSpy responseSpy(&secIFace, &Zera::cSECInterface::serverAnswer);
    int msgNr = secIFace.getChannelCatalog();
    feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ec0;ec1;ec2;ec3;ec4;ec5;ec6;ec7;"));
}

void test_servicemock::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
