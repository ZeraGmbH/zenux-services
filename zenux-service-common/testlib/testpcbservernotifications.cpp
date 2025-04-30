#include "testpcbservernotifications.h"
#include "permissionfunctions.h"

TestPcbServerNotifications::TestPcbServerNotifications(SettingsContainerPtr settings,
                                                       AbstractFactoryI2cCtrlPtr ctrlFactory,
                                                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory) :
    PCBServer(std::move(settings), tcpNetworkFactory)
{
    m_scpiConnectionList.append(this);
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);
}

TestPcbServerNotifications::~TestPcbServerNotifications()
{
}

void TestPcbServerNotifications::insertScpiConnection(ScpiConnection *scpiConnection)
{
    m_scpiConnectionList.append(scpiConnection);
}

void TestPcbServerNotifications::initTestSCPIConnections()
{
    initSCPIConnections();
}

void TestPcbServerNotifications::registerNotifier(QString inputCmd, int notifierId)
{
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg("SERVER:REGISTER").arg(inputCmd).arg(notifierId);
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(m_scpiInterface->getSCPIObject("SERVER:REGISTER"));
    scpiDelegate->executeSCPI(protoCmd);
}

void TestPcbServerNotifications::unregisterNotifier()
{
    QString scpiAuthorizationQuery = QString("%1 %2;").arg("SERVER:UNREGISTER").arg("");
    ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject("SERVER:UNREGISTER");
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
}

void TestPcbServerNotifications::doConfiguration()
{
}

void TestPcbServerNotifications::onNotifySubscriber(ScpiNotificationSubscriber subscriber, QString newValue)
{
    Q_UNUSED(newValue)
    emit notificationSent(subscriber.m_notifierId);
}
