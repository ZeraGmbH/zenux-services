#include "pcbtestserver.h"
#include "adjustmentstatusnull.h"

PCBTestServer::PCBTestServer(QString name, QString version) :
    cPCBServer(name, version)
{
    m_adjustmentStatusNull = new AdjustmentStatusNull();
    scpiConnectionList.append(m_pStatusInterface = new cStatusInterface(getSCPIInterface(), m_adjustmentStatusNull));
    initSCPIConnections();
}

void PCBTestServer::doConfiguration()
{
}
