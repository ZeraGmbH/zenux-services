#include "pcbtestserver.h"
#include "permissionfunctions.h"

PCBTestServer::PCBTestServer(QString name, QString version, cSCPI *scpiInterface, MockAtmel *atmel) :
    cPCBServer(name, version, scpiInterface)
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

void PCBTestServer::doConfiguration()
{
}

void PCBTestServer::onSendNotification(ScpiNotificationSubscriber subscriber)
{
    emit notificationSent(subscriber.m_notifierId);
}
