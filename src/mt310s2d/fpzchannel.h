#ifndef FPZCHANNEL_H
#define FPZCHANNEL_H

#include <QObject>

#include <scpiconnection.h>
#include <notificationstring.h>

namespace FPZChannel
{
enum Commands
{
    cmdAlias,
    cmdType,
    cmdDspServer,
    cmdDspChannel,
    cmdStatus,
    cmdFormFactor,
    cmdConstant,
    cmdPowtype
};


const double FormFactor = 5.6294995e6; // fout = (Pact/Pnenn) * FPZnenn * FormFactor
}


namespace SourceSystem
{
    class cChannelSettings;
}

class cFPZChannel : public cSCPIConnection
{
    Q_OBJECT

public:
    cFPZChannel(cSCPI* scpiinterface, QString description, quint8 nr, SourceSystem::cChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes);

    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    quint16 m_nDspServer; // the dsp servers port
    quint8 m_nDspChannel; // where to find the channel's sampled data
    quint8 m_nType;
    double m_fFormFactor;
    bool m_bAvail; // is this channel available ?

    cNotificationString notifierConstant;
    void initNotifier(cNotificationString& notifier);
    cNotificationString notifierPowerType;

    QString m_ReadAlias(QString& sInput);
    QString m_ReadType(QString& sInput);
    QString m_ReadDspServer(QString& sInput);
    QString m_ReadDspChannel(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
    QString m_ReadFFactor(QString& sInput);
    QString m_ReadWriteConstant(QString& sInput);
    QString m_ReadWritePowerType(QString& sInput);
};

#endif // FPZCHANNEL_H
