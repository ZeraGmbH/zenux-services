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

quint32 cRMInterfacePrivate::rmIdent(QString name)
{
    quint32 msgnr;

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::NetReply* message = envelope.mutable_reply();
    message->set_rtype(ProtobufMessage::NetMessage::NetReply::IDENT);
    message->set_body(name.toStdString());

    msgnr = m_clientSmart->transmitCommand(&envelope);
    m_MsgNrCmdList[msgnr] = rmident;

    return msgnr;
}


quint32 cRMInterfacePrivate::addResource(QString type, QString name, int n, QString description, quint16 port)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:ADD"), par = QString("%1;%2;%3;%4;%5;")
            .arg(type, name)
            .arg(n)
            .arg(description)
            .arg(port));

    m_MsgNrCmdList[msgnr] = addresource;
    return msgnr;
}


quint32 cRMInterfacePrivate::removeResource(QString type, QString name)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:REM"), par = QString("%1;%2;").arg(type, name));

    m_MsgNrCmdList[msgnr] = removeresource;
    return msgnr;
}


quint32 cRMInterfacePrivate::getResourceTypes()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:TYPE:CAT?"));

    m_MsgNrCmdList[msgnr] = getresourcetypes;
    return msgnr;
}


quint32 cRMInterfacePrivate::getResources(QString type)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:%1:CAT?").arg(type));

    m_MsgNrCmdList[msgnr] = getresources;
    return msgnr;
}


quint32 cRMInterfacePrivate::getResourceInfo(QString type, QString name)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:%1:%2?").arg(type, name));

    m_MsgNrCmdList[msgnr] = getresources;
    return msgnr;
}


quint32 cRMInterfacePrivate::setResource(QString type, QString name, int n)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:%1:%2").arg(type, name), par = QString("SET;%1;").arg(n));

    m_MsgNrCmdList[msgnr] = setresource;
    return msgnr;
}


quint32 cRMInterfacePrivate::freeResource(QString type, QString name)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("RES:%1:%2").arg(type, name) , par = QString("FREE;"));

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
