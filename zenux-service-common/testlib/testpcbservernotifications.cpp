#include "testpcbservernotifications.h"
#include "permissionfunctions.h"

TestPcbServerNotifications::TestPcbServerNotifications(SettingsContainerPtr settings, cSCPI *scpiInterface, AbstractFactoryI2cCtrlPtr ctrlFactory) :
    PCBServer(std::move(settings), scpiInterface)
{
    scpiConnectionList.append(this);
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);
}

TestPcbServerNotifications::~TestPcbServerNotifications()
{
}

void TestPcbServerNotifications::insertScpiConnection(ScpiConnection *scpiConnection)
{
    scpiConnectionList.append(scpiConnection);
}

void TestPcbServerNotifications::initTestSCPIConnections()
{
    initSCPIConnections();
}

void TestPcbServerNotifications::registerNotifier(QString inputCmd, int notifierId)
{
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg("SERVER:REGISTER").arg(inputCmd).arg(notifierId);
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(getSCPIInterface()->getSCPIObject("SERVER:REGISTER"));
    scpiDelegate->executeSCPI(protoCmd);
}

void TestPcbServerNotifications::unregisterNotifier()
{
    QString scpiAuthorizationQuery = QString("%1 %2;").arg("SERVER:UNREGISTER").arg("");
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIObject* scpiObject = getSCPIInterface()->getSCPIObject("SERVER:UNREGISTER");
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
}

void TestPcbServerNotifications::doConfiguration()
{
}

void TestPcbServerNotifications::onSendNotification(ScpiNotificationSubscriber subscriber)
{
    emit notificationSent(subscriber.m_notifierId);
}
