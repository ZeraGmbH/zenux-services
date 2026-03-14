#include "rminterface_p.h"
#include "rminterface.h"
#include "variantconverter.h"
#include <netmessages.pb.h>

namespace Zera {

cRMInterfacePrivate::cRMInterfacePrivate(cRMInterface *iface)
    :q_ptr(iface)
{
}

void cRMInterfacePrivate::setClientSmart(Zera::ProxyClientPtr client)
{
    m_clientSmart = client;
    connect(m_clientSmart.get(), &Zera::ProxyClient::answerAvailable, this, &cRMInterfacePrivate::receiveAnswer);
    connect(m_clientSmart.get(), &Zera::ProxyClient::tcpError, this, &cRMInterfacePrivate::receiveError);
}

quint32 cRMInterfacePrivate::scpiCommand(const QString &scpi)
{
    quint32 msgnr;
    QList<QString> sl = scpi.split(' ');
    if (sl.count() <= 1)
        msgnr = sendCommand(scpi);
    else
        msgnr = sendCommand(sl.at(0), sl.at(1));

    m_MsgNrCmdList[msgnr] = rmscpi;
    return msgnr;
}

quint32 cRMInterfacePrivate::rmIdent(const QString &name)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::NetReply* message = envelope.mutable_reply();
    message->set_rtype(ProtobufMessage::NetMessage::NetReply::IDENT);
    message->set_body(name.toStdString());

    quint32 msgnr = m_clientSmart->transmitCommand(&envelope);
    m_MsgNrCmdList[msgnr] = rmident;

    return msgnr;
}

quint32 cRMInterfacePrivate::addResource(const QString &type, const QString &name, int n, const QString &description, quint16 port)
{
    quint32 msgnr = sendCommand(QString("RES:ADD"), QString("%1;%2;%3;%4;%5;")
            .arg(type, name)
            .arg(n)
            .arg(description)
            .arg(port));

    m_MsgNrCmdList[msgnr] = addresource;
    return msgnr;
}

quint32 cRMInterfacePrivate::removeResource(const QString &type, const QString &name)
{
    quint32 msgnr = sendCommand(QString("RES:REM"), QString("%1;%2;").arg(type, name));
    m_MsgNrCmdList[msgnr] = removeresource;
    return msgnr;
}

quint32 cRMInterfacePrivate::getResourceTypes()
{
    quint32 msgnr = sendCommand(QString("RES:TYPE:CAT?"));
    m_MsgNrCmdList[msgnr] = getresourcetypes;
    return msgnr;
}

quint32 cRMInterfacePrivate::getResources(const QString &type)
{
    quint32 msgnr = sendCommand(QString("RES:%1:CAT?").arg(type));
    m_MsgNrCmdList[msgnr] = getresources;
    return msgnr;
}

quint32 cRMInterfacePrivate::getResourceInfo(const QString &type, const QString &name)
{
    quint32 msgnr = sendCommand(QString("RES:%1:%2?").arg(type, name));
    m_MsgNrCmdList[msgnr] = getresources;
    return msgnr;
}

quint32 cRMInterfacePrivate::setResource(const QString &type, const QString &name, int n)
{
    quint32 msgnr = sendCommand(QString("RES:%1:%2").arg(type, name), QString("SET;%1;").arg(n));
    m_MsgNrCmdList[msgnr] = setresource;
    return msgnr;
}

quint32 cRMInterfacePrivate::freeResource(const QString &type, const QString &name)
{
    quint32 msgnr = sendCommand(QString("RES:%1:%2").arg(type, name) , QString("FREE;"));
    m_MsgNrCmdList[msgnr] = freeresource;
    return msgnr;
}

void cRMInterfacePrivate::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    TAnswerDecoded decodedAnswer;
    if(decodeProtobuffAnswer(message, decodedAnswer)) {
        Q_Q(cRMInterface);
        switch (decodedAnswer.cmdSendEnumVal) {
        case rmident:
        case addresource:
        case removeresource:
        case setresource:
        case freeresource:
        case rmscpi:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnString(decodedAnswer.msgBody));
            break;
        case getresourcetypes:
        case getresources:
        case getresourceinfo:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnString(decodedAnswer.msgBody));
            break;
        }
    }
}

void cRMInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cRMInterface);
    emit q->tcpError(errorCode);
}

}
