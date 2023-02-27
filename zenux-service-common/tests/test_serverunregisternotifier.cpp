#include "statusinterface.h"
#include "accumulatorinterface.h"
#include "test_serverunregisternotifier.h"
#include "atmelsyscntrltest.h"
#include "foutgroupresourceandinterface.h"
#include <QTest>

QTEST_MAIN(test_serverunregisternotifier)

static const char *statusAuthorizationCommand = "STATUS:AUTHORIZATION?";
static const char *accumulatorStatusCommand ="SYSTEM:ACCUMULATOR:STATUS?";

constexpr quint16 NOTIFICATION_ID = 1;

test_serverunregisternotifier::test_serverunregisternotifier() :
    m_scpiInterface("foo")
{
}

void test_serverunregisternotifier::init()
{
    m_atmel = std::make_unique<MockAtmel>();
    m_adjustmentStatusNull = std::make_unique<AdjustmentStatusNull>();
    m_pcbServerTest = std::make_unique<PCBTestServer>("foo", "0", &m_scpiInterface, m_atmel.get());

    m_xmlConfigReader = std::make_unique<Zera::XMLConfig::cReader>();
    m_settings = std::make_unique<FOutSettings>(m_xmlConfigReader.get());
    connect(m_xmlConfigReader.get(), &Zera::XMLConfig::cReader::valueChanged,
            m_settings.get(), &FOutSettings::configXMLInfo);
    m_xmlConfigReader->loadSchema(QStringLiteral(CONFIG_PATH) + "/" + "mt310s2d.xsd");
    m_xmlConfigReader->loadXMLFile(QStringLiteral(CONFIG_PATH) + "/" + "mt310s2d.xml");
}

cSCPIDelegate *test_serverunregisternotifier::getDelegate(QString cmd)
{
    cSCPIObject* scpiObject = m_pcbServerTest->getSCPIInterface()->getSCPIObject(cmd);
    return static_cast<cSCPIDelegate*>(scpiObject);
}

void test_serverunregisternotifier::oneScpiConnection()
{
    m_pcbServerTest->insertScpiConnection(new cStatusInterface(m_pcbServerTest->getSCPIInterface(), m_adjustmentStatusNull.get()));
    m_pcbServerTest->initTestSCPIConnections();
    m_pcbServerTest->registerNotifier(statusAuthorizationCommand, NOTIFICATION_ID);
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 1);
    m_pcbServerTest->unregisterNotifier();
    QCOMPARE(getDelegate(statusAuthorizationCommand)->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 0);
}

void test_serverunregisternotifier::twoScpiConnections()
{
    m_pcbServerTest->insertScpiConnection(new cStatusInterface(m_pcbServerTest->getSCPIInterface(), m_adjustmentStatusNull.get()));

    AtmelSysCntrlTest atmelSysCtrl("", 0, 0);
    m_pcbServerTest->insertScpiConnection(new AccumulatorInterface(m_pcbServerTest->getSCPIInterface(), &atmelSysCtrl));

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
    QList<FOutSettings::ChannelSettings*>& chSettings =  m_settings->getChannelSettings();
    QCOMPARE(chSettings.size(), 4);
}

void test_serverunregisternotifier::scpiConnectionWithInternalScpiConnections()
{
    m_pcbServerTest->insertScpiConnection(new FOutGroupResourceAndInterface(m_pcbServerTest->getSCPIInterface(), m_settings.get()));
    m_pcbServerTest->initTestSCPIConnections();

    m_pcbServerTest->registerNotifier("SOURCE:fo1:CONSTANT?", NOTIFICATION_ID);
    QCOMPARE(getDelegate("SOURCE:fo1:CONSTANT?")->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 1);
    m_pcbServerTest->registerNotifier("SOURCE:fo3:CONSTANT?", NOTIFICATION_ID);
    QCOMPARE(getDelegate("SOURCE:fo3:CONSTANT?")->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 1);

    m_pcbServerTest->unregisterNotifier();
    QCOMPARE(getDelegate("SOURCE:fo1:CONSTANT?")->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 0);
    QCOMPARE(getDelegate("SOURCE:fo3:CONSTANT?")->getScpiNotificationSubscriberHandler().getTotalSubscribers(), 0);
}