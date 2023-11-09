#ifndef SECCHANNEL_H
#define SECCHANNEL_H

#include "scpiconnection.h"
#include "secinputsettings.h"
#include "seccalcsettings.h"
#include "fpgasettings.h"
#include "notificationvalue.h"
#include "protonetcommand.h"
#include <QList>
#include <QByteArray>
#include <functional>

#define baseChnName "ec"

// some bit definitions
#define en_n 8
#define en_p 9
#define direction 15
#define bitSingle 16
#define sssto 17 // sync steuerung stop ?
#define ssarm 19 // sync steuerung (sorry hardware is german) arm
#define imp_en 21
#define impout_en 22
#define cnt2carry 23


namespace ECALCREG {
    enum { CMD, CONF, STATUS, INTMASK, INTREG, MTCNTin, MTCNTfin, MTCNTact, MTPULSin = 12, MTPAUSEin, MTPULS, MTPAUSE};
}

namespace ECALCCMDID {
    enum { COUNTEDGE = 1, COUNTRESET, ERRORMEASMASTER, ERRORMEASSLAVE};
}

namespace ECalcChannel
{
enum Commands
{
    cmdRegister,
    setSync,
    setMux,
    setCmdid,
    start,
    stop,
    intAcknowledge
};

}

class SecChannel : public ScpiConnection
{
    Q_OBJECT
public:
    SecChannel(int devFileDescriptor,
               SecCalculatorSettings* esettings,
               FPGASettings* fsettings,
               SecInputSettings* inpsettings,
               quint16 nr,
               std::function<void(int)> funcSigHandler);
    ~SecChannel();
    virtual void initSCPIConnection(QString leadingNodes) override;

    QString& getName();
    bool isfree();
    bool set(QByteArray id);
    void free();
    void setIntReg(quint8 reg);

    void m_StopErrorCalculator();
    void resetInterrupt(quint8 interrupt);

protected:
    virtual void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    int m_devFileDescriptor;
    SecCalculatorSettings* m_pecalcsettings;
    FPGASettings* m_pFPGASettings;
    SecInputSettings* m_pInputSettings;
    quint16 m_nNr;
    quint32 m_nBaseAdress;
    quint32 m_nMyAdress;

    QList<quint32> CMDIDList;

    QString m_sName; // the channels name ec0...
    bool m_bSet; // we mark if the channel is occupied
    QByteArray m_ClientId; // we remark the clientid for arbitration purpose
    void m_ReadWriteRegister(cProtonetCommand* protoCmd);
    void m_setSync(cProtonetCommand* protoCmd);
    void m_setMux(cProtonetCommand* protoCmd);
    void m_setCmdId(cProtonetCommand* protoCmd);
    void m_start(cProtonetCommand* protoCmd);
    void m_stop(cProtonetCommand* protoCmd);
    void m_resetInt(cProtonetCommand* protoCmd);

    NotificationValue notifierECalcChannelIntReg;
    std::function<void(int)> m_funcSigHandler;

};

#endif // SECCHANNEL_H
