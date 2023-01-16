#ifndef FPZINCHANNELINTERFACE_H
#define FPZINCHANNELINTERFACE_H

#include "scpiconnection.h"

namespace FPZINChannel
{
enum Commands
{
    cmdAlias,
    cmdStatus
};

}


namespace FRQInputSystem
{
    class cChannelSettings;
}

class FpzInChannelInterface : public ScpiConnection
{
    Q_OBJECT

public:
    FpzInChannelInterface(cSCPI* scpiInterface, QString description, quint8 nr, FRQInputSystem::cChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes) override;

    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    bool m_bAvail; // is this channel available ?

    QString m_ReadAlias(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
};

#endif // FPZINCHANNELINTERFACE_H
