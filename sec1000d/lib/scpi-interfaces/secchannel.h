#ifndef SECCHANNEL_H
#define SECCHANNEL_H

#include "abstractfactorydevicenodepcb.h"
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
               AbstractFactoryDeviceNodeSecPtr deviceNodeFactory);
    ~SecChannel();
    void initSCPIConnection(const QString &leadingNodes);

    QString getName() const;
    bool isfree();
    bool set(QByteArray id);
    void free();
    void setIntReg(quint8 reg);

    void m_StopErrorCalculator();
    void resetInterrupt(quint8 interrupt);

protected:
    virtual void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;

private:
    AbstractFactoryDeviceNodeSecPtr m_deviceNodeFactory;
    SecCalculatorSettings* m_pecalcsettings;
    SecInputSettings* m_pInputSettings;
    quint16 m_nNr;
    quint32 m_nMyAdress;

    QList<quint32> CMDIDList;

    QString m_sName; // the channels name ec0...
    bool m_bSet; // we mark if the channel is occupied
    QByteArray m_ClientId; // we remark the clientid for arbitration purpose
    void m_ReadWriteRegister(ProtonetCommandPtr protoCmd);
    void m_setSync(ProtonetCommandPtr protoCmd);
    void m_setMux(ProtonetCommandPtr protoCmd);
    void m_setCmdId(ProtonetCommandPtr protoCmd);
    void m_start(ProtonetCommandPtr protoCmd);
    void m_stop(ProtonetCommandPtr protoCmd);
    void m_resetInt(ProtonetCommandPtr protoCmd);

    NotificationValue notifierECalcChannelIntReg;
    std::function<void(int)> m_funcSigHandler;

    static constexpr const char* baseChnName = "ec";
    static constexpr int en_n = 8;
    static constexpr int en_p = 9;
    static constexpr int direction = 15;
    static constexpr int bitSingle = 16;
    static constexpr int sssto = 17;
    static constexpr int ssarm = 19;
    static constexpr int imp_en = 21;
    static constexpr int impout_en = 22;
    static constexpr int cnt2carry = 23;
};

#endif // SECCHANNEL_H
