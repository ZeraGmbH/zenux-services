#ifndef SECGROUPRESOURCEANDINTERFACE_H
#define SECGROUPRESOURCEANDINTERFACE_H

#include "resource.h"
#include "secchannel.h"
#include "seccalcsettings.h"
#include "secinputsettings.h"
#include "protonetcommand.h"
#include <QList>
#include <QHash>
#include <functional>

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

class SecGroupResourceAndInterface : public cResource
{
    Q_OBJECT

public:
    SecGroupResourceAndInterface(SecCalculatorSettings* ecalcSettings,
                                 SecInputSettings* inputsettings,
                                 std::function<void(int)> funcSigHandler);
    ~SecGroupResourceAndInterface();
    void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
    QList<SecChannel*> getECalcChannelList();

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    SecCalculatorSettings* m_pecalcsettings;
    SecInputSettings* m_pInputSettings;

    QString m_sVersion;

    QList<SecChannel*> m_ECalculatorChannelList;
    QHash<QString,SecChannel*> m_ECalculatorChannelHash;
    QHash<QByteArray, QString> m_ClientECalcHash; // we hold the set ecalculators by clientid

    QString m_ReadVersion(QString& sInput);
    QString m_ReadECalculatorChannelCatalog(QString& sInput);
    void m_SetChannels(cProtonetCommand *protoCmd);
    void m_FreeChannels(cProtonetCommand *protoCmd);
};

#endif // SECGROUPRESOURCEANDINTERFACE_H
