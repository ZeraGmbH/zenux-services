#include "secinterface_p.h"
#include "secinterface.h"
#include "variantconverter.h"
#include <netmessages.pb.h>

namespace Zera
{

cSECInterfacePrivate::cSECInterfacePrivate(cSECInterface *iface)
    :q_ptr(iface)
{
}

void cSECInterfacePrivate::setClientSmart(ProxyClientPtr client)
{
    if (m_clientSmart) // we avoid multiple connections
        disconnect(m_clientSmart.get(), 0, this, 0);
    m_clientSmart = client;
    connect(m_clientSmart.get(), &Zera::ProxyClient::answerAvailable, this, &cSECInterfacePrivate::receiveAnswer);
    connect(m_clientSmart.get(), &Zera::ProxyClient::tcpError, this, &cSECInterfacePrivate::receiveError);
}

quint32 cSECInterfacePrivate::setECalcUnit(int n)
{
    quint32 msgnr = sendCommand(QString("ECAL:SET"), QString("%1;").arg(n));
    m_MsgNrCmdList[msgnr] = SEC::setecalcunit;
    return msgnr;
}

quint32 cSECInterfacePrivate::freeECalcUnits()
{
    quint32 msgnr = sendCommand(QString("ECAL:FREE"));
    m_MsgNrCmdList[msgnr] = SEC::freeecalcunit;
    return msgnr;
}

quint32 cSECInterfacePrivate::writeRegister(const QString &chnname, quint8 reg, quint32 value)
{
    quint32 msgnr = sendCommand(QString("ECAL:%1:R%2").arg(chnname).arg(reg), QString("%1;").arg(value));
    m_MsgNrCmdList[msgnr] = SEC::writeregister;
    return msgnr;
}

quint32 cSECInterfacePrivate::readRegister(const QString &chnname, quint8 reg)
{
    quint32 msgnr = sendCommand(QString("ECAL:%1:R%2?").arg(chnname).arg(reg));
    m_MsgNrCmdList[msgnr] = SEC::readregister;
    return msgnr;
}

quint32 cSECInterfacePrivate::setSync(const QString &chnname, const QString &syncChn)
{
    quint32 msgnr = sendCommand(QString("ECAL:%1:SYNC").arg(chnname), QString("%1;").arg(syncChn));
    m_MsgNrCmdList[msgnr] = SEC::setsync;
    return msgnr;
}

quint32 cSECInterfacePrivate::setMux(const QString &chnname, const QString &inpname)
{
    quint32 msgnr = sendCommand(QString("ECAL:%1:MUX").arg(chnname), QString("%1;").arg(inpname));
    m_MsgNrCmdList[msgnr] = SEC::setmux;
    return msgnr;
}

quint32 cSECInterfacePrivate::setCmdid(const QString &chnname, quint8 cmdid)
{
    quint32 msgnr = sendCommand(QString("ECAL:%1:CMDID").arg(chnname), QString("%1;").arg(cmdid));
    m_MsgNrCmdList[msgnr] = SEC::setcmdid;
    return msgnr;
}

quint32 cSECInterfacePrivate::start(const QString &chnname)
{
    quint32 msgnr = sendCommand(QString("ECAL:%1:START").arg(chnname));
    m_MsgNrCmdList[msgnr] = SEC::startecalc;
    return msgnr;
}

quint32 cSECInterfacePrivate::stop(const QString &chnname)
{
    quint32 msgnr = sendCommand(QString("ECAL:%1:STOP").arg(chnname));
    m_MsgNrCmdList[msgnr] = SEC::stopecalc;
    return msgnr;
}

quint32 cSECInterfacePrivate::intAck(const QString &chnname, quint8 interrupt)
{
    quint32 msgnr = sendCommand(QString("ECAL:%1:INT").arg(chnname), QString("%1;").arg(interrupt));
    m_MsgNrCmdList[msgnr] = SEC::intacknowledge;
    return msgnr;
}

quint32 cSECInterfacePrivate::registerNotifier(const QString &query)
{
    quint32 msgnr = sendCommand(QString("SERV:REG"), QString("%1;").arg(query));
    m_MsgNrCmdList[msgnr] = SEC::regnotifier;
    return msgnr;
}

quint32 cSECInterfacePrivate::unregisterNotifiers()
{
    quint32 msgnr = sendCommand(QString("SERV:UNR;"));
    m_MsgNrCmdList[msgnr] = SEC::unregnotifier;
    return msgnr;
}

quint32 cSECInterfacePrivate::transparentCommand(const QString &cmd)
{
    QList<QString> sl = cmd.split(' ');
    quint32 msgnr;
    if (sl.count() <= 1)
        msgnr = sendCommand(cmd);
    else
        msgnr = sendCommand(sl.at(0), sl.at(1));

    m_MsgNrCmdList[msgnr] = SEC::transparentcommand;
    return msgnr;
}

void cSECInterfacePrivate::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    if (message->has_reply()) {
        quint32 lmsgnr = message->messagenr();
        QString lmsg = "";
        if (message->reply().has_body())
            lmsg = QString::fromStdString(message->reply().body());

        int lreply = message->reply().rtype();
        int lastCmd = m_MsgNrCmdList.take(lmsgnr);

        Q_Q(cSECInterface);

        switch (lastCmd)
        {
        case SEC::setecalcunit:
        case SEC::freeecalcunit:
        case SEC::writeregister:
        case SEC::readregister:
        case SEC::setsync:
        case SEC::setmux:
        case SEC::setcmdid:
        case SEC::startecalc:
        case SEC::stopecalc:
        case SEC::regnotifier:
        case SEC::unregnotifier:
        case SEC::intacknowledge:
        case SEC::transparentcommand:
            emit q->serverAnswer(lmsgnr, lreply, VariantConverter::returnString(lmsg));
            break;

        }
    }
}

void cSECInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cSECInterface);
    emit q->tcpError(errorCode);
}

}
