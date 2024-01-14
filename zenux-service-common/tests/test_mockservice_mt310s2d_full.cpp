#include "test_mockservice_mt310s2d_full.h"
#include "pcbinterface.h"
#include "proxy.h"
#include "mockatmelctrlfactory.h"
#include "mockpcbserver.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_mt310s2d_full)

void test_mockservice_mt310s2d_full::initTestCase()
{
    //qputenv("QT_FATAL_CRITICALS", "1");
}

void test_mockservice_mt310s2d_full::init()
{
    m_resman = std::make_unique<ResmanRunFacade>();
    TimeMachineObject::feedEventLoop();
    std::shared_ptr<SettingsForDeviceServer> settings = std::make_shared<SettingsForDeviceServer>(MockPcbServer::createParams("mt310s2d"));
    m_mt310s2d = std::make_unique<MockMt310s2dFull>(settings, std::make_shared<MockAtmelCtrlFactory>(true));
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_mt310s2d_full::cleanup()
{
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_mt310s2d_full::connectServer()
{
    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(pcbClient);

    QSignalSpy connectSpy(pcbClient.get(), &Zera::ProxyClient::connected);
    Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    TimeMachineObject::feedEventLoop();
    //QCOMPARE(connectSpy.count(), 1);
}
