#include "tinyserverscpicmdinterface.h"
#include "variantconverter.h"

TinyServerScpiCmdInterface::TinyServerScpiCmdInterface(Zera::ProxyClientPtr client) :
    m_clientSmart(client)
{
    connect(m_clientSmart.get(), &Zera::ProxyClient::answerAvailable, this, &TinyServerScpiCmdInterface::receiveAnswer);
    connect(m_clientSmart.get(), &Zera::ProxyClient::tcpError, this, &TinyServerScpiCmdInterface::tcpError);
}

quint32 TinyServerScpiCmdInterface::scpiCommand(const QString &scpi)
{
    QList<QString> sl = scpi.split(' ');
    if (sl.count() > 1)
        return sendCommand(sl.at(0), sl.at(1));
    return sendCommand(scpi);
}

quint32 TinyServerScpiCmdInterface::sendCommand(const QString &cmd)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    return m_clientSmart->transmitCommand(&envelope);
}

quint32 TinyServerScpiCmdInterface::sendCommand(const QString &cmd, const QString &par)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());
    return m_clientSmart->transmitCommand(&envelope);
}

void TinyServerScpiCmdInterface::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    if (message->has_reply()) {
        quint32 lmsgnr = message->messagenr();

        QString lmsg = "";
        if (message->reply().has_body())
            lmsg = QString::fromStdString(message->reply().body());

        int lreply = message->reply().rtype();
        emit serverAnswer(lmsgnr, lreply, Zera::VariantConverter::returnString(lmsg));
    }
}
