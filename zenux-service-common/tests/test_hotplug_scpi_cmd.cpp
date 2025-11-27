#include "test_hotplug_scpi_cmd.h"
#include "testfactoryi2cctrl.h"
#include <mocktcpnetworkfactory.h>
#include "proxy.h"
#include "reply.h"
#include "controllerpersitentdata.h"
#include "emobdefinitions.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_hotplug_scpi_cmd)

void test_hotplug_scpi_cmd::init()
{
    setupServers();
}

void test_hotplug_scpi_cmd::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
}

void test_hotplug_scpi_cmd::readErrorStatusNoParamNoEmob()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:ERROR?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_hotplug_scpi_cmd::readErrorStatusNoParamEmobIAUX()
{
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IAUX");
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:ERROR?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(errorInstrumentStatus::Instrument_Status_Cable_Error));
}

void test_hotplug_scpi_cmd::readErrorStatusIAUXEmobI3IAUX()
{
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:ERROR? m7;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(errorInstrumentStatus::Instrument_Status_Cable_Error));
}

void test_hotplug_scpi_cmd::readErrorStatusI3EmobIAUX()
{
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:ERROR? m5;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_hotplug_scpi_cmd::clearErrorStatusNoParamNoEmob()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:CLEARERROR;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_hotplug_scpi_cmd::clearErrorStatusNoParamEmobIAUX()
{
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IAUX");
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:CLEARERROR;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));
}

void test_hotplug_scpi_cmd::clearErrorStatusIAUXEmobI3IAUX()
{
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:CLEARERROR m7;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));
}

void test_hotplug_scpi_cmd::clearErrorStatusI3EmobIAUX()
{
    m_mt310s2d->addStandardEmobControllers(QStringList() << "IAUX");

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:CLEARERROR m5;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_hotplug_scpi_cmd::readDataNoHotplug()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:READDATA?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_hotplug_scpi_cmd::readDataOneEmobIAUX()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL1", {"EMOB_MOCK-00V00", cClamp::undefined});
    m_mt310s2d->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:READDATA?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
}

void test_hotplug_scpi_cmd::readDataMt650eI3EmobIAUX()
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    infoMap.insert("IL3", {"MT650e", cClamp::undefined});
    infoMap.insert("IAUX", {"EMOB_MOCK-00V00", cClamp::undefined});
    m_mt310s2d->fireHotplugInterrupt(infoMap);
    TimeMachineObject::feedEventLoop();

    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    int msgNr = m_pcbIFace->scpiCommand("SYSTEM:EMOB:READDATA?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("Data"));
}

void test_hotplug_scpi_cmd::setupServers()
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
