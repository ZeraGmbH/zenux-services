#ifndef PROXYCLIENTFORNETMESSAGETEST_H
#define PROXYCLIENTFORNETMESSAGETEST_H

#include <proxyclient.h>
#include <QList>

class ProxyClientForNetMessageTest : public Zera::ProxyClient
{
    Q_OBJECT
public:
    quint32 transmitCommand(ProtobufMessage::NetMessage *message) override;
    struct ScpiParts {
        QString cmd;
        QString param;
    };
    const QList<ScpiParts> &getTransmittedMessages() const;
private:
    QList<ScpiParts> m_transmittedMessages;
};

#endif // PROXYCLIENTFORNETMESSAGETEST_H
