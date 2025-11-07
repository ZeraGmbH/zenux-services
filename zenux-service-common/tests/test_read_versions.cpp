#include "test_read_versions.h"
#include "testfactoryi2cctrl.h"
#include <mocktcpnetworkfactory.h>
#include "proxy.h"
#include "reply.h"
#include "controllerpersitentdata.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_read_versions)

void test_read_versions::init()
{
    setupServers();
}

void test_read_versions::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
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
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/controller-versions/noEmob.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_versions::readPcbVersionOneEmobChannelIAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IAUX");
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:PCB?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/controller-versions/oneEmobChannelIAUX.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_versions::readPcbVersionTwoEmobTwoChannels()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IL3" << "IAUX");
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:PCB?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/controller-versions/twoEmobtwoChannels.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
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
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/controller-versions/ctrlVersionNoEmob.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_versions::readCtrlVersionOneEmobChannelIAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IAUX");
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:CTRL?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/controller-versions/ctrlVersionOneEmobOneChannel.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_versions::readCtrlVersionTwoEmobTwoChannels()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IL3" << "IAUX");
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:VERSION:CTRL?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

    QByteArray jsonDumped = responseSpy[0][2].toByteArray();
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/controller-versions/ctrlVersiontwoEmobtwoChannels.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_versions::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, "mt310s2d");
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();

}
