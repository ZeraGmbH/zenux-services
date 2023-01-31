#ifndef PCBTESTSERVER_H
#define PCBTESTSERVER_H

#include "pcbserver.h"
#include "statusinterface.h"

class PCBTestServer : public cPCBServer
{
public:
    PCBTestServer(QString name, QString version);
private slots:
    void doConfiguration();
private:
    cStatusInterface *m_pStatusInterface;
    AdjustmentStatusInterface *m_adjustmentStatusNull;
};

#endif // PCBTESTSERVER_H
