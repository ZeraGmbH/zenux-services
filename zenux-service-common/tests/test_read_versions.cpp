#include "test_read_versions.h"
#include "accustatusflags.h"
#include "testfactoryi2cctrl.h"
#include <mocktcpnetworkfactory.h>
#include "proxy.h"
#include "reply.h"
#include "controllerpersitentdata.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_read_versions)

void test_read_versions::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_read_versions::init()
{
    TimeMachineForTest::reset();
    setupServers();
}

void test_read_versions::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
    m_notificationsReceivedCtrl.clear();
    m_notificationsReceivedPcb.clear();
}

void test_read_versions::readPcbVersionNoEmob()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:PCB?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/noEmob.json", jsonDumped));
}

void test_read_versions::readPcbVersionOneEmobChannelIAUX()
{
    registerNotifications();
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IAUX");
    unregisterNotifications();
    QCOMPARE(m_notificationsReceivedPcb.count(), 1);

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:PCB?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/oneEmobChannelIAUX.json", jsonDumped));
}

void test_read_versions::readPcbVersionOneEmobAddAndRemoveIAUX()
{
    registerNotifications();
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IAUX");
    m_mt310s2d->removeAllHotplugDevices();
    unregisterNotifications();
    QCOMPARE(m_notificationsReceivedPcb.count(), 2);

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:PCB?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/noEmob.json", jsonDumped));
}

void test_read_versions::readPcbVersionTwoEmobTwoChannels()
{
    registerNotifications();
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    unregisterNotifications();
    // add passes bootloader / delay... => each channel fires
    QCOMPARE(m_notificationsReceivedPcb.count(), 2);

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:PCB?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/twoEmobtwoChannels.json", jsonDumped));
}

void test_read_versions::readPcbVersionAccuNoEmob()
{
    ControllerPersitentData::getData().m_accuStatus |= (1<<bp_Battery_Present);
    waitForAccuDetected();

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:PCB?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/noEmobAccu.json", jsonDumped));
}

void test_read_versions::readCtrlVersionNoEmob()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:CTRL?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/ctrlVersionNoEmob.json", jsonDumped));
}

void test_read_versions::readCtrlVersionOneEmobChannelIAUX()
{
    registerNotifications();
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IAUX");
    unregisterNotifications();
    QCOMPARE(m_notificationsReceivedCtrl.count(), 1);

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:CTRL?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/ctrlVersionOneEmobOneChannel.json", jsonDumped));
}

void test_read_versions::readCtrlVersionOneEmobAddRemoveIAUX()
{
    registerNotifications();
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IAUX");
    m_mt310s2d->removeAllHotplugDevices();
    unregisterNotifications();
    QCOMPARE(m_notificationsReceivedCtrl.count(), 2);

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:CTRL?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/ctrlVersionNoEmob.json", jsonDumped));
}

void test_read_versions::readCtrlVersionTwoEmobTwoChannels()
{
    registerNotifications();
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    unregisterNotifications();
    // add passes bootloader / delay... => each channel fires
    QCOMPARE(m_notificationsReceivedCtrl.count(), 2);

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:CTRL?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/ctrlVersiontwoEmobtwoChannels.json", jsonDumped));
}

void test_read_versions::waitForAccuDetected()
{
    constexpr int ACCU_POLLING_PERIOD_MS = 1000;
    TimeMachineForTest::getInstance()->processTimers(ACCU_POLLING_PERIOD_MS);
}

void test_read_versions::readCtrlVersionAccuNoEmob()
{
    ControllerPersitentData::getData().m_accuStatus |= (1<<bp_Battery_Present);
    waitForAccuDetected();

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:CTRL?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/controller-versions/ctrlVersionAccuNoEmob.json", jsonDumped));
}

constexpr int ctrlVersionNotifierId = 5000;
constexpr int pcbVersionNotifierId  = 5001;

void test_read_versions::catchInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer)
{
    Q_UNUSED(reply)
    if (msgnr == 0) { // 0 was reserved for async. messages
        int notifierID = answer.toString().split(':').last().toInt();
        if(notifierID == ctrlVersionNotifierId)
            m_notificationsReceivedCtrl.append(notifierID);
        else if(notifierID == pcbVersionNotifierId)
            m_notificationsReceivedPcb.append(notifierID);
    }
}

void test_read_versions::registerNotifications()
{
    m_pcbIFace->registerNotifier(QString("SYSTEM:VERSION:CTRL?"), ctrlVersionNotifierId);
    m_pcbIFace->registerNotifier(QString("SYSTEM:VERSION:PCB?"), pcbVersionNotifierId);
    TimeMachineObject::feedEventLoop();
    connect(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer,
            this, &test_read_versions::catchInterfaceAnswer);
}

void test_read_versions::unregisterNotifications()
{
    m_pcbIFace->unregisterNotifiers();
    TimeMachineObject::feedEventLoop();
}

void test_read_versions::setupServers()
{
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), m_tcpNetworkFactory, "mt310s2d");
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, m_tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

