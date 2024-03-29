#include "proxyclientfortest.h"
#include <reply.h>

namespace Zera {

std::shared_ptr<ProxyClientForTest> ProxyClientForTest::create()
{
    return std::make_shared<ProxyClientForTest>();
}

ProxyClientForTest::ProxyClientForTest()
{
    connect(this, &ProxyClientForTest::sigQueueAnswer,
            this, &ProxyClientForTest::onQueueAnswer,
            Qt::QueuedConnection);
}

void ProxyClientForTest::setAnswers(ServerTestAnswers answers)
{
    m_answers = answers;
}

quint32 ProxyClientForTest::transmitCommand(ProtobufMessage::NetMessage *message)
{
    storeMessage(message);
    if(!m_answers.isEmpty()) {
        ServerTestAnswer answer = m_answers.take();
        if(answer.answerType !=ServerTestAnswer::TCP_ERROR) {
            quint32 msgId = pushAnswer(message, answer);
            emit sigQueueAnswer();
            return msgId;
        }
        else {
            emit tcpError(QAbstractSocket::RemoteHostClosedError);
            return 0;
        }
    }
    return 0;
}

QStringList ProxyClientForTest::getReceivedIdents() const
{
    return m_receivedIdents;
}

QStringList ProxyClientForTest::getReceivedCommands() const
{
    return m_receivedCommands;
}

void ProxyClientForTest::onQueueAnswer()
{
    emit answerAvailable(m_pendingNetAnswers.takeFirst());

}

quint32 ProxyClientForTest::pushAnswer(ProtobufMessage::NetMessage *message, ServerTestAnswer answer)
{
    std::shared_ptr<ProtobufMessage::NetMessage> answerMessage = std::make_shared<ProtobufMessage::NetMessage>(*message);
    ProtobufMessage::NetMessage::NetReply *answerReply = answerMessage->mutable_reply();
    answerReply->set_body(answer.answer.toString().toStdString());
    setReply(answerReply, answer);
    quint32 messageNr = calcMessageNr(answer, answerMessage.get());
    m_pendingNetAnswers.append(answerMessage);
    return messageNr;
}

void ProxyClientForTest::storeMessage(ProtobufMessage::NetMessage *message)
{
    if(message->has_reply() &&
           message->reply().rtype() == ProtobufMessage::NetMessage::NetReply::IDENT)
        m_receivedIdents.append(QString::fromStdString(message->reply().body()));
    if(message->has_scpi())
        storeScpi(message);
}

void ProxyClientForTest::storeScpi(ProtobufMessage::NetMessage *message)
{
    ProtobufMessage::NetMessage::ScpiCommand scpiCmd = message->scpi();
    QString strCmd = QString::fromStdString(scpiCmd.command());
    QString param = QString::fromStdString(scpiCmd.parameter());
    if(!param.isEmpty())
        strCmd += " " + param;
    m_receivedCommands.append(strCmd);
}

quint32 ProxyClientForTest::calcMessageNr(ServerTestAnswer answer, ProtobufMessage::NetMessage* answerMessage)
{
    quint32 messageNr = 0;
    switch(answer.answerType)
    {
    default:
        messageNr = m_msgIds.nextId();
        answerMessage->set_messagenr(messageNr);
        break;
    case ServerTestAnswer::MSG_ID_OTHER:
        messageNr = m_msgIds.nextId();
        answerMessage->set_messagenr(-messageNr);
        break;
    case ServerTestAnswer::MSG_ID_INTERRUPT:
        messageNr = 0;
        answerMessage->set_messagenr(messageNr);
        break;
    }
    return messageNr;
}

void ProxyClientForTest::setReply(ProtobufMessage::NetMessage::NetReply *answerReply, ServerTestAnswer answer)
{
    switch(answer.reply) {
    case ack:
        answerReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
        break;
    case nack:
        answerReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
        break;
    default:
        answerReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
        break;
    }
}

}
