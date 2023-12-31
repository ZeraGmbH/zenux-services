#include "pcbtestserver.h"
#include "permissionfunctions.h"

PCBTestServer::PCBTestServer(ServerParams params, cSCPI *scpiInterface, AtmelPermissionMock *atmel) :
    cPCBServer(params, scpiInterface)
{
    scpiConnectionList.append(this);
    PermissionFunctions::setPermissionPinController(atmel);
}

PCBTestServer::~PCBTestServer()
{
}

void PCBTestServer::insertScpiConnection(ScpiConnection *scpiConnection)
{
    scpiConnectionList.append(scpiConnection);
}

void PCBTestServer::initTestSCPIConnections()
{
    initSCPIConnections();
}

void PCBTestServer::registerNotifier(QString inputCmd, int notifierId)
{
    QString scpiAuthorizationQuery = QString("%1 %2;%3;").arg("SERVER:REGISTER").arg(inputCmd).arg(notifierId);
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(getSCPIInterface()->getSCPIObject("SERVER:REGISTER"));
    scpiDelegate->executeSCPI(protoCmd);
}

void PCBTestServer::unregisterNotifier()
{
    QString scpiAuthorizationQuery = QString("%1 %2;").arg("SERVER:UNREGISTER").arg("");
    cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, false, QByteArray(), 0, scpiAuthorizationQuery);
    cSCPIObject* scpiObject = getSCPIInterface()->getSCPIObject("SERVER:UNREGISTER");
    cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
    scpiDelegate->executeSCPI(protoCmd);
}

void PCBTestServer::doConfiguration()
{
}

void PCBTestServer::onSendNotification(ScpiNotificationSubscriber subscriber)
{
    emit notificationSent(subscriber.m_notifierId);
}
