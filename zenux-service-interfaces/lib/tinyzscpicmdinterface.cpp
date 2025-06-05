#include "tinyzscpicmdinterface.h"
#include "variantconverter.h"

TinyZScpiCmdInterface::TinyZScpiCmdInterface(Zera::ProxyClientPtr client) :
    m_clientSmart(client)
{
    connect(m_clientSmart.get(), &Zera::ProxyClient::answerAvailable, this, &TinyZScpiCmdInterface::receiveAnswer);
    connect(m_clientSmart.get(), &Zera::ProxyClient::tcpError, this, &TinyZScpiCmdInterface::tcpError);
}

quint32 TinyZScpiCmdInterface::scpiCommand(const QString &scpi)
{
    QString scpiWork = scpi.trimmed();
    int firstSpacePos = scpiWork.indexOf(" ");

    QString cmd;
    QString param;
    if (firstSpacePos > 0) {
        cmd = scpiWork.left(firstSpacePos);
        param = scpiWork.right(scpi.length()-firstSpacePos-1);
    }
    else
        cmd = scpiWork;

    if(cmd.endsWith(";"))
        cmd = cmd.left(cmd.length()-1);
    if(param.endsWith(";"))
        param = param.left(param.length()-1);

    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(param.toStdString());
    return m_clientSmart->transmitCommand(&envelope);
}

void TinyZScpiCmdInterface::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
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
