#ifndef ECALCINTERFACE_H
#define ECALCINTERFACE_H

#include "resource.h"
#include "scpiconnection.h"
#include "ecalcchannel.h"
#include "notificationvalue.h"
#include "ethsettingssec.h"
#include <QObject>
#include <QList>
#include <QHash>

namespace ECalcSystem
{

const QString Version = "V1.00";
const QString sECalculatorDescription = "Standard error calculator base unit";

enum Commands
{
    cmdVersion,
    cmdChannelCat,
    cmdSetChannels,
    cmdFreeChannels
};

}

class cSEC1000dServer;
class cECalculatorChannel;
class cECalculatorSettings;
class FPGASettings;
class cInputSettings;
class cProtonetCommand;


class cECalculatorInterface : public cResource
{
    Q_OBJECT

public:
    cECalculatorInterface(cSEC1000dServer* server, EthSettingsSec* ethsettings, cECalculatorSettings* ecalcSettings, FPGASettings* fpgasettings, cInputSettings* inputsettings);
    ~cECalculatorInterface();
    void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
    QList<cECalculatorChannel*> getECalcChannelList();

protected:
    void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    cSEC1000dServer* m_pMyServer;
    EthSettingsSec* m_pETHsettings;
    cECalculatorSettings* m_pecalcsettings;
    FPGASettings* m_pFPGASettings;
    cInputSettings* m_pInputSettings;

    QString m_sVersion;

    QList<cECalculatorChannel*> m_ECalculatorChannelList;
    QHash<QString,cECalculatorChannel*> m_ECalculatorChannelHash;
    QHash<QByteArray, QString> m_ClientECalcHash; // we hold the set ecalculators by clientid

    QString m_ReadVersion(QString& sInput);
    QString m_ReadECalculatorChannelCatalog(QString& sInput);
    void m_SetChannels(cProtonetCommand *protoCmd);
    void m_FreeChannels(cProtonetCommand *protoCmd);
};

#endif // ECALCINTERFACE_H
