#include "proxyclient_p.h"
#include "proxyclient.h"
#include "proxy.h"
#include "proxy_p.h"
#include <netmessages.pb.h>

namespace Zera {

int ProxyClientPrivate::m_instanceCount = 0;

ProxyClientPrivate::ProxyClientPrivate(ProxyPrivate* proxy) : m_pProxy(proxy)
{
    m_instanceCount++;
    setParent(proxy);
}

ProxyClientPrivate::~ProxyClientPrivate()
{
    m_instanceCount--;
    Proxy::getInstance()->releaseConnection(this);
}

void ProxyClientPrivate::transmitAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    emit answerAvailable(message);
}

void ProxyClientPrivate::transmitError(QAbstractSocket::SocketError errorCode)
{
    emit tcpError(errorCode);
}

void ProxyClientPrivate::transmitConnection()
{
    emit connected();
}

void ProxyClientPrivate::transmitDisConnection()
{
    emit disconnected();
}

int ProxyClientPrivate::getInstanceCount()
{
    return m_instanceCount;
}

quint32 ProxyClientPrivate::transmitCommand(ProtobufMessage::NetMessage *message)
{
    return m_pProxy->transmitCommand(this, message);
}

}
