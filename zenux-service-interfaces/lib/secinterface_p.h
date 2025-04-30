#ifndef SECINTERFACE_P_H
#define SECINTERFACE_P_H


#include <QObject>
#include <QStringList>

#include "interface_p.h"
#include "secinterface.h"

namespace Zera { namespace SEC {
enum seccommands
{
    setecalcunit,
    freeecalcunit,
    writeregister,
    readregister,
    setsync,
    setmux,
    setcmdid,
    startecalc,
    stopecalc,
    regnotifier,
    unregnotifier,
    intacknowledge,
    transparentcommand
};
}

class cSECInterface;

class cSECInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT
public:
    cSECInterfacePrivate(cSECInterface* iface);
    void setClientSmart(Zera::ProxyClientPtr client);
    quint32 setECalcUnit(int n); // we want n eclac units, answer is the names of them
    quint32 freeECalcUnits(); // free all the units the client had set
    quint32 writeRegister(QString chnname, quint8 reg, quint32 value); // transp. register access
    quint32 readRegister(QString chnname, quint8 reg);

    quint32 setSync(QString chnname, QString syncChn);
    quint32 setMux(QString chnname, QString inpname);
    quint32 setCmdid(QString chnname, quint8 cmdid);
    quint32 start(QString chnname);
    quint32 stop(QString chnname);
    quint32 intAck(QString chnname, quint8 interrupt);

    quint32 registerNotifier(QString query); // register for notification on change
    quint32 unregisterNotifiers(); // unregister from all notifications

    // sending a transparent command
    quint32 transparentCommand(QString cmd);

protected slots:
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) override;
    void receiveError(QAbstractSocket::SocketError errorCode) override;

private:
    Q_DECLARE_PUBLIC(cSECInterface)
    cSECInterface *q_ptr;
};

}

#endif // SECINTERFACE_P_H
