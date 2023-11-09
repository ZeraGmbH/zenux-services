#include "test_servicemock.h"
#include "mockpcbserver.h"
#include "resmanrunfacade.h"
#include "foutgroupresourceandinterface.h"
#include "proxy.h"
#include "pcbinterface.h"
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
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    ResourceSettingsList resourcesWithConfig;
    resourcesWithConfig.append({&foutSettings, &foutResource});
    mock.setResourcesWithConfig(resourcesWithConfig);

    mock.start();
    feedEventLoop();
    QVERIFY(mock.areAllResourcesConnected());
}

void test_servicemock::connectMt310s2d()
{
    ResmanRunFacade resman;
    MockPcbServer mock("mt310s2d");
    FOutSettings foutSettings(mock.getConfigReader());
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    ResourceSettingsList resourcesWithConfig;
    resourcesWithConfig.append({&foutSettings, &foutResource});
    mock.setResourcesWithConfig(resourcesWithConfig);

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
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    ResourceSettingsList resourcesWithConfig;
    resourcesWithConfig.append({&foutSettings, &foutResource});
    mock.setResourcesWithConfig(resourcesWithConfig);

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
    FOutGroupResourceAndInterface foutResource(mock.getSCPIInterface(), &foutSettings);
    ResourceSettingsList resourcesWithConfig;
    resourcesWithConfig.append({&foutSettings, &foutResource});
    mock.setResourcesWithConfig(resourcesWithConfig);

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

void test_servicemock::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
