#include "test_mockservice_zdsp1d.h"
#include "proxy.h"
#include "dspinterface.h"
#include "reply.h"
#include <QAbstractEventDispatcher>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_zdsp1d)

void test_mockservice_zdsp1d::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_mockservice_zdsp1d::init()
{
    m_resman = std::make_unique<ResmanRunFacade>();
    feedEventLoop();
    m_zsdp1d = std::make_unique<MockZdsp1d>();
    feedEventLoop();
}

void test_mockservice_zdsp1d::cleanup()
{
    m_zsdp1d = nullptr;
    feedEventLoop();
    m_resman = nullptr;
    feedEventLoop();
}

void test_mockservice_zdsp1d::getDspLcaVersion()
{
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

void test_mockservice_zdsp1d::getServerVersion()
{
    Zera::ProxyClientPtr dspClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6310);
    Zera::cDSPInterface dspIFace;
    dspIFace.setClientSmart(dspClient);

    Zera::Proxy::getInstance()->startConnectionSmart(dspClient);
    feedEventLoop();

    QSignalSpy responseSpy(&dspIFace, &Zera::cDSPInterface::serverAnswer);
    int msgNr = dspIFace.readServerVersion();
    feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("zdsp1d V1.11"));
}

void test_mockservice_zdsp1d::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
