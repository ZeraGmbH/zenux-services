#include "pcbtestserver.h"
#include "adjustmentstatusnull.h"
#include "permissionfunctions.h"

PCBTestServer::PCBTestServer(QString name, QString version, cSCPI *scpiInterface, MockAtmel *atmel) :
    cPCBServer(name, version, scpiInterface)
{
    m_adjustmentStatusNull = new AdjustmentStatusNull();
    scpiConnectionList.append(this);
    scpiConnectionList.append(m_pStatusInterface = new cStatusInterface(getSCPIInterface(), m_adjustmentStatusNull));
    initSCPIConnections();
    PermissionFunctions::setPermissionPinController(atmel);
}

PCBTestServer::~PCBTestServer()
{
    if (m_adjustmentStatusNull) delete m_adjustmentStatusNull;
    if (m_pStatusInterface) delete m_pStatusInterface;
}

void PCBTestServer::sendNotificationToClient(NotificationStructWithStringAndId notData)
{
    emit notificationSent(notData.notifierId);
}

void PCBTestServer::doConfiguration()
{
}
