#include "test_generator_scpi.h"
#include "testfactoryi2cctrl.h"
#include "proxy.h"
#include "reply.h"
#include "controllerpersitentdata.h"
#include "scpisingletransactionblocked.h"
#include <mocktcpnetworkfactory.h>
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_generator_scpi);

void test_generator_scpi::init()
{
    setupServers();
}

void test_generator_scpi::cleanup()
{
    m_pcbIFace = nullptr;
    m_proxyClient = nullptr;
    m_mt310s2d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    ControllerPersitentData::cleanupPersitentData();
}

void test_generator_scpi::getSetValidSourceModeOn()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(""));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON m0,m3;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("m0,m3"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(""));
}

void test_generator_scpi::getSetInvalidSourceModeOn()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON m0,m21;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(error));
    QCOMPARE(responseSpy[0][2], QVariant("errexec"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(""));
}

void test_generator_scpi::getSetValidSourceOn()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:SWITCHON?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(""));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:SWITCHON m0,m3;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:SWITCHON?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("m0,m3"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:SWITCHON;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:SWITCHON?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(""));
}

void test_generator_scpi::setChangeRangeByAmplitude()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);

    m_pcbIFace->scpiCommand("GENERATOR:m2:AMPRANGE 2.5;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));

    QCOMPARE(channelChangeProbablySpy.count(), 1);
    QCOMPARE(channelChangeProbablySpy[0][0], "m2");
}

void test_generator_scpi::queryRangeByAmplitudeDenied()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);

    m_pcbIFace->scpiCommand("GENERATOR:m2:AMPRANGE?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));

    QCOMPARE(channelChangeProbablySpy.count(), 0);
}

void test_generator_scpi::setChangeRangeByAmplitudeInvalidChannel()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);
    m_pcbIFace->scpiCommand("GENERATOR:m6:AMPRANGE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));

    QCOMPARE(channelChangeProbablySpy.count(), 0);
}

void test_generator_scpi::setChangeRangeByAmplitudeInvalidValue()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);
    m_pcbIFace->scpiCommand("GENERATOR:m2:AMPRANGE foo;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));

    QCOMPARE(channelChangeProbablySpy.count(), 0);
}

void test_generator_scpi::getSetRange()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);

    responseSpy.clear();
    channelChangeProbablySpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:RANGE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("0"));
    QCOMPARE(channelChangeProbablySpy.count(), 0);

    responseSpy.clear();
    channelChangeProbablySpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:RANGE 1;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));
    QCOMPARE(channelChangeProbablySpy.count(), 1);
    QCOMPARE(channelChangeProbablySpy[0][0], "m0");

    responseSpy.clear();
    channelChangeProbablySpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:RANGE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("1"));
    QCOMPARE(channelChangeProbablySpy.count(), 0);
}

void test_generator_scpi::getRangeInvalidChannel()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);

    m_pcbIFace->scpiCommand("GENERATOR:m6:RANGE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
    QCOMPARE(channelChangeProbablySpy.count(), 0);
}

void test_generator_scpi::setRangeInvalidChannel()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);

    m_pcbIFace->scpiCommand("GENERATOR:m6:RANGE 1;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(nack));
    QCOMPARE(responseSpy[0][2], QVariant("nak"));
    QCOMPARE(channelChangeProbablySpy.count(), 0);
}


static constexpr int notifierId = 42;

void test_generator_scpi::notifySourceModeOnChange()
{
    m_pcbIFace->registerNotifier("GENERATOR:MODEON?", notifierId, true);
    TimeMachineObject::feedEventLoop();
    QSignalSpy spy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);

    m_pcbIFace->scpiCommand("GENERATOR:MODEON m0,m3;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2 /* notification first + change mode on response */);
    QString notificationStr = spy[0][2].toString();
    QCOMPARE(notificationStr, "Notify:42:m0,m3");

    spy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:MODEON;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spy.count(), 2);
    notificationStr = spy[0][2].toString();
    QCOMPARE(notificationStr, "Notify:42:");
}

void test_generator_scpi::getSetDspAmplitude()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSAMPLITUDE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("0"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSAMPLITUDE 1.5;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSAMPLITUDE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("1.5"));
    QCOMPARE(channelChangeProbablySpy.count(), 0);
}

void test_generator_scpi::getSetDspFrequency()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSFREQUENCY?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("0"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSFREQUENCY 50;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSFREQUENCY?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("50"));
    QCOMPARE(channelChangeProbablySpy.count(), 0);
}

void test_generator_scpi::getSetDspAngle()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &AbstractServerInterface::serverAnswer);
    QSignalSpy channelChangeProbablySpy(m_mt310s2d->getGeneratorInterface(), &GeneratorInterface::sigMeasRangeProbablyChanged);

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSANGLE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("0"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSANGLE 45;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ack"));

    responseSpy.clear();
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSANGLE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("45"));
    QCOMPARE(channelChangeProbablySpy.count(), 0);
}

void test_generator_scpi::noCrossWrite()
{
    m_pcbIFace->scpiCommand("GENERATOR:MODEON m0,m3;");
    m_pcbIFace->scpiCommand("GENERATOR:SWITCHON m0,m2;");
    m_pcbIFace->scpiCommand("GENERATOR:m0:RANGE 1;");
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSAMPLITUDE 1.5;");
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSFREQUENCY 50;");
    m_pcbIFace->scpiCommand("GENERATOR:m0:DSANGLE 45;");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(ScpiSingleTransactionBlocked::query("GENERATOR:MODEON?"), "m0,m3");
    QCOMPARE(ScpiSingleTransactionBlocked::query("GENERATOR:SWITCHON?"), "m0,m2");
    QCOMPARE(ScpiSingleTransactionBlocked::query("GENERATOR:m0:RANGE?"), "1");
    QCOMPARE(ScpiSingleTransactionBlocked::query("GENERATOR:m0:DSAMPLITUDE?"), "1.5");
    QCOMPARE(ScpiSingleTransactionBlocked::query("GENERATOR:m0:DSFREQUENCY?"), "50");
    QCOMPARE(ScpiSingleTransactionBlocked::query("GENERATOR:m0:DSANGLE?"), "45");
}

void test_generator_scpi::setupServers()
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, "mt581s2d");
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}
