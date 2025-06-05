#include "proxyclientfornetmessagetest.h"

quint32 ProxyClientForNetMessageTest::transmitCommand(ProtobufMessage::NetMessage *message)
{
    m_transmittedMessages.append({
        QString::fromStdString(message->scpi().command()),
        QString::fromStdString(message->scpi().parameter())
    });
    return 0;
}

const QList<ProxyClientForNetMessageTest::ScpiParts> &ProxyClientForNetMessageTest::getTransmittedMessages() const
{
    return m_transmittedMessages;
}
