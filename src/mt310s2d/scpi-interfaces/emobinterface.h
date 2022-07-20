#ifndef EMOBINTERFACE_H
#define EMOBINTERFACE_H

#include "mt310s2d.h"
#include <scpiconnection.h>
#include <notificationstring.h>
#include <protonetcommand.h>

class EmobInterface: public cSCPIConnection
{
public:
    enum SystemCommands
    {
        cmdChannelCat,
        cmdVersionCTRL,
    };

    EmobInterface(cI2CSettings *i2cSettings, cSCPI* pSCPIInterface);
    void initSCPIConnection(QString leadingNodes) override;
    void actualizeClampStatus(quint16 devConnectedMask);
protected slots:
    void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString readChannelCatalog(QString scpiCmd);

    cI2CSettings *m_i2cSettings;
    cNotificationString m_notifierChannelList;
};

#endif // EMOBINTERFACE_H
