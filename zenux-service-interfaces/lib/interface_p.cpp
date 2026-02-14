#include "interface_p.h"
#include <proxy.h>
#include <QStringList>

namespace Zera {

int cInterfacePrivate::m_instanceCount = 0;

int cInterfacePrivate::getInstanceCount()
{
    return m_instanceCount;
}

cInterfacePrivate::cInterfacePrivate()
{
    m_instanceCount++;
}

cInterfacePrivate::~cInterfacePrivate()
{
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_clientSmart); // no async. messages anymore
    m_instanceCount--;
}

bool cInterfacePrivate::decodeProtobuffAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message,
                                              TAnswerDecoded &decodedAnswer,
                                              int interruptEnumVal)
{
    bool withReply = message->has_reply();
    if(withReply) {
        decodedAnswer.msgNr = message->messagenr();
        if (message->reply().has_body())
            decodedAnswer.msgBody = QString::fromStdString(message->reply().body());
        decodedAnswer.reply = message->reply().rtype();
        if(interruptEnumVal != -1 && decodedAnswer.msgNr == 0)
            decodedAnswer.cmdSendEnumVal = interruptEnumVal;
        else
            decodedAnswer.cmdSendEnumVal = m_MsgNrCmdList.take(decodedAnswer.msgNr);
    }
    return withReply;
}

quint32 cInterfacePrivate::sendCommand(QString cmd)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    return m_clientSmart->transmitCommand(&envelope);
}

quint32 cInterfacePrivate::sendCommand(QString cmd, QString par)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());
    return m_clientSmart->transmitCommand(&envelope);
}

}

