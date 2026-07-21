#ifndef SECCHANNEL_H
#define SECCHANNEL_H

#include "abstractfactorydevicenodesec.h"
#include "scpiconnection.h"
#include "secinputsettings.h"
#include "seccalcsettings.h"
#include "notificationvalue.h"
#include "protonetcommand.h"
#include <QList>
#include <QByteArray>
#include <functional>

class SecChannel : public ScpiConnection
{
    Q_OBJECT
public:
    SecChannel(std::shared_ptr<cSCPI> scpiInterface,
               SecCalculatorSettings* esettings,
               SecInputSettings* inpsettings,
               quint16 nr,
               std::function<void(int)> funcSigHandler,
               AbstractFactoryDeviceNodeSecPtr zdspSupportFactory);
    void initSCPIConnection(const QString &leadingNodes);

    QString getName() const;
    bool isfree() const;
    bool set(const QByteArray &id);
    void free();
    void setIntReg(quint8 reg);

    void stopErrorCalculator();
    void resetInterrupt(quint8 interrupt);

private:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
    void readWriteRegister(const ProtonetCommandPtr &protoCmd);
    void setSync(const ProtonetCommandPtr &protoCmd);
    void setMux(const ProtonetCommandPtr &protoCmd);
    void setCmdId(const ProtonetCommandPtr &protoCmd);
    void start(const ProtonetCommandPtr &protoCmd);
    void stop(const ProtonetCommandPtr &protoCmd);
    void resetInt(const ProtonetCommandPtr &protoCmd);

    AbstractFactoryDeviceNodeSecPtr m_zdspSupportFactory;
    SecCalculatorSettings* m_pecalcsettings;
    SecInputSettings* m_pInputSettings;
    quint16 m_nNr;
    quint32 m_nMyAdress;

    QList<quint32> CMDIDList;

    QString m_sName; // the channels name ec0...
    bool m_bSet; // we mark if the channel is occupied
    QByteArray m_ClientId; // we remark the clientid for arbitration purpose

    NotificationValue notifierECalcChannelIntReg;
    std::function<void(int)> m_funcSigHandler;
};

#endif // SECCHANNEL_H
