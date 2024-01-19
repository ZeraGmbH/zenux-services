#include "statusinterface.h"
#include "test_serverunregisternotifier.h"
#include "testfactoryi2cctrl.h"
#include "accumulatorinterface.h"
#include "foutgroupresourceandinterface.h"
#include <QTest>

QTEST_MAIN(test_serverunregisternotifier)

static const char *statusAuthorizationCommand = "STATUS:AUTHORIZATION?";
static const char *accumulatorStatusCommand ="SYSTEM:ACCUMULATOR:STATUS?";

constexpr quint16 NOTIFICATION_ID = 1;

void test_serverunregisternotifier::init()
{
    static ServerParams params {"foo", "0", QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd", QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml"};
    
    m_adjustmentStatusNull = std::make_unique<TestAdjustmentStatusInterfaceNull>();
    m_ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true);
    m_pcbServerTest = std::make_unique<TestPcbServerNotifications>(params, &m_scpiInterface, m_ctrlFactory);

    m_xmlConfigReader = std::make_unique<Zera::XMLConfig::cReader>();
    m_foutSettings = std::make_unique<FOutSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(), &Zera::XMLConfig::cReader::valueChanged, m_foutSettings.get(), &FOutSettings::configXMLInfo);
    m_accSettings = std::make_unique<AccumulatorSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(), &Zera::XMLConfig::cReader::valueChanged, m_accSettings.get(), &AccumulatorSettings::configXMLInfo);
    m_xmlConfigReader->loadSchema(params.xsdFile);
    m_xmlConfigReader->loadXMLFile(params.xmlFile);
}

cSCPIDelegate *test_serverunregisternotifier::getDelegate(QString cmd)
{
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(cmd);
    return static_cast<cSCPIDelegate*>(scpiObject);
}

void test_serverunregisternotifier::oneScpiConnection()
{
    m_pcbServerTest->insertScpiConnection(new cStatusInterface(m_pcbServerTest->getSCPIInterface(), m_adjustmentStatusNull.get(), m_ctrlFactory));
    m_pcbServerTest->initTestSCPIConnections();

    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 1);
    m_pcbServerTest->unregisterNotifier();
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 0);
}

void test_serverunregisternotifier::twoScpiConnections()
{
    m_pcbServerTest->insertScpiConnection(new cStatusInterface(m_pcbServerTest->getSCPIInterface(), m_adjustmentStatusNull.get(), m_ctrlFactory));
    m_pcbServerTest->insertScpiConnection(new AccumulatorInterface(m_pcbServerTest->getSCPIInterface(), m_accSettings.get(), m_ctrlFactory));
    m_pcbServerTest->initTestSCPIConnections();

    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    m_pcbServerTest->registerNotifier(accumulatorStatusCommand, NOTIFICATION_ID);
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 1);
    QCOMPARE(getDelegate(accumulatorStatusCommand)->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 1);
    m_pcbServerTest->unregisterNotifier();
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 0);
    QCOMPARE(getDelegate(accumulatorStatusCommand)->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 0);
}

void test_serverunregisternotifier::mtConfigLoaded()
{
    QList<FOutSettings::ChannelSettings*>& chSettings =  m_foutSettings->getChannelSettings();
    QCOMPARE(chSettings.size(), 4);
}

void test_serverunregisternotifier::scpiConnectionWithInternalScpiConnections()
{
    m_pcbServerTest->insertScpiConnection(new FOutGroupResourceAndInterface(m_pcbServerTest->getSCPIInterface(), m_foutSettings.get()));
    m_pcbServerTest->initTestSCPIConnections();

    m_pcbServerTest->registerNotifier("SOURCE:fo1:CONSTANT?", NOTIFICATION_ID);
    QCOMPARE(getDelegate("SOURCE:fo1:CONSTANT?")->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 1);
    m_pcbServerTest->registerNotifier("SOURCE:fo3:CONSTANT?", NOTIFICATION_ID);
    QCOMPARE(getDelegate("SOURCE:fo3:CONSTANT?")->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 1);

    m_pcbServerTest->unregisterNotifier();
    QCOMPARE(getDelegate("SOURCE:fo1:CONSTANT?")->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 0);
    QCOMPARE(getDelegate("SOURCE:fo3:CONSTANT?")->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 0);
}
