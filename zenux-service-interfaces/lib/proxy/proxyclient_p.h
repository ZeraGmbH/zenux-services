#ifndef PROXYCLIENT_P_H
#define PROXYCLIENT_P_H

#include <proxyclient.h>

namespace Zera {

class ProxyPrivate;

class ProxyClientPrivate: public ProxyClient
{
    Q_OBJECT
public:
    ProxyClientPrivate(ProxyPrivate* proxy);
    virtual ~ProxyClientPrivate();
    void transmitAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message);
    void transmitError(QAbstractSocket::SocketError errorCode);
    void transmitDisConnection();

    static int getInstanceCount();
public slots:
    void transmitConnection();
protected:
    virtual quint32 transmitCommand(ProtobufMessage::NetMessage *message) override;
    ProxyPrivate *m_pProxy;
    static int m_instanceCount;
};

}
#endif // PROXYCLIENT_P_H
