#include "test_emob_lock.h"
#include "testfactoryi2cctrl.h"
#include <mocktcpnetworkfactory.h>
#include "proxy.h"
#include "reply.h"
#include "controllerpersitentdata.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_emob_lock)

void test_emob_lock::init()
{
    setupServers();
}

void test_emob_lock::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
}

void test_emob_lock::pushButtonNoParamNoEmob()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:PBPRESS;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_emob_lock::pushButtonNoParamEmobIAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:PBPRESS;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));
}

void test_emob_lock::pushButtonNoParamEmobI3IAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IL3" << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:PBPRESS;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_emob_lock::pushButtonIAUXNoEmob()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:PBPRESS m7;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_emob_lock::pushButtonIAUXEmobIAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:PBPRESS m7;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));
}

void test_emob_lock::pushButtonI3EmobI3()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IL3");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:PBPRESS m5;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));
}

void test_emob_lock::pushButtonI3EmobIAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:PBPRESS m5;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_emob_lock::readLockStateNoParamNoEmob()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:LOCKST?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_emob_lock::readLockStateNoParamEmobIAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:LOCKST?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("1"));
}

void test_emob_lock::readLockStateNoParamEmobI3IAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IL3" << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:LOCKST?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_emob_lock::readLockStateIAUXNoEmob()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:LOCKST? m7;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_emob_lock::readLockStateIAUXEmobIAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:LOCKST? m7;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("1"));
}

void test_emob_lock::readLockStateI3EmobI3()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IL3");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:LOCKST? m5;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("1"));
}

void test_emob_lock::readLockStateI3EmobIAUX()
{
    m_mt310s2d->fireHotplugInterrupt(QStringList() << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:LOCKST? m5;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}


void test_emob_lock::setupServers()
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
