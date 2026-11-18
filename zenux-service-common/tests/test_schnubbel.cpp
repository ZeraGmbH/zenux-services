#include "test_schnubbel.h"
#include "testfactoryi2cctrl.h"
#include "controllerpersitentdata.h"
#include "proxy.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <mockeepromdevice.h>
#include <mocktcpnetworkfactory.h>
#include <tcpnetworkfactory.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_schnubbel)

void test_schnubbel::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    // For now with real network to have protobuf in
    m_tcpNetworkFactory = VeinTcp::TcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_schnubbel::initTestCase_data()
{
    QTest::addColumn<QString>("serviceNameForAlternateDevice");
    QTest::newRow("MT310s2") << QString("mt310s2d");
    QTest::newRow("MT581s2") << QString("mt581s2d");
}

void test_schnubbel::init()
{
    QFETCH_GLOBAL(QString, serviceNameForAlternateDevice);
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpNetworkFactory, serviceNameForAlternateDevice);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_schnubbel::cleanup()
{
    MockEepromDevice::cleanAll();
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_schnubbel::schnubbelPollWorking()
{
    QStringList schnubbelStatesReceived;
    constexpr int schnubbelNotifierID = 36;
    connect(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer, this, [&](quint32 msgnr, quint8 reply, const QVariant &answer) {
        Q_UNUSED(reply)
        if (msgnr==0) {
            QStringList entries = answer.toString().split(':');
            QCOMPARE(entries.count(), 3);
            QCOMPARE(entries[0], "Notify");
            QCOMPARE(entries[1].toInt(), schnubbelNotifierID);
            schnubbelStatesReceived.append(entries[2]);
        }
    });
    m_pcbIFace->registerNotifier("STATUS:AUTHORIZATION?", schnubbelNotifierID, true);

    TimeMachineForTest::getInstance()->processTimers(5000);
    QCOMPARE(schnubbelStatesReceived.count(), 1);
    QCOMPARE(schnubbelStatesReceived[0], "1");

    ControllerPersitentData::getData().m_permission = false;
    TimeMachineForTest::getInstance()->processTimers(5000);
    QCOMPARE(schnubbelStatesReceived.count(), 2);
    QCOMPARE(schnubbelStatesReceived[1], "0");
}

