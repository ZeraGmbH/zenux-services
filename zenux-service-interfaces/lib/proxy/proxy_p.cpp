#include "proxy_p.h"
#include "proxyclient_p.h"
#include "proxynetpeer.h"
#include "proxyconnection.h"
#include "notzeronumgen.h"
#include <QUuid>
#include <vtcp_peer.h>
#include <netmessages.pb.h>

namespace Zera {

Proxy* ProxyPrivate::m_singletonInstance = nullptr;

ProxyPrivate::ProxyPrivate(Proxy *parent):
    q_ptr(parent)
{
}

ProxyClientPtr ProxyPrivate::getConnectionSmart(QString ipadress, quint16 port,
                                                VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory)
{
    QUuid uuid = QUuid::createUuid(); // we use a per client uuid
    QByteArray binUUid = uuid.toRfc4122();

    ProxyNetPeer *proxyPeer = getProxyNetPeer(ipadress, port, tcpNetworkFactory);
    ProxyClientPrivate *proxyclient = new ProxyClientPrivate(this);
    ProxyConnection *connection = new ProxyConnection(ipadress, port, binUUid, proxyPeer);
    m_ConnectionHash[proxyclient] = connection;
    m_ClientHash[binUUid] = proxyclient;
    return ProxyClientPtr(proxyclient);
}

void ProxyPrivate::startConnectionSmart(ProxyClientPtr client)
{
    ProxyClientPrivate* castHackClient = static_cast<ProxyClientPrivate*>(client.get());
    ProxyConnection *connection = m_ConnectionHash[castHackClient];
    ProxyNetPeer *peer = connection->m_pNetClient;
    if(peer->isStarted()) {
        if(peer->isConnected())
            castHackClient->transmitConnection();
    }
    else
        peer->startProxyConnection(connection->m_sIP, connection->m_nPort);
}

bool ProxyPrivate::releaseConnection(ProxyClientPrivate *client)
{
    if(m_ConnectionHash.contains(client)) {
        ProxyConnection *connection = m_ConnectionHash.take(client);
        QByteArray binUUid = m_ClientHash.key(client);
        m_ClientHash.remove(binUUid);

        ProtobufMessage::NetMessage netCommand;
        ProtobufMessage::NetMessage::NetCmd *Command = netCommand.mutable_netcommand();
        Command->set_cmd(ProtobufMessage::NetMessage_NetCmd_CmdType_RELEASE);
        netCommand.set_clientid(binUUid.data(), binUUid.size());
        connection->m_pNetClient->sendMessage(m_protobufWrapper.protobufToByteArray(netCommand));
        delete connection;
        return true;
    }
    return false;
}

quint32 ProxyPrivate::transmitCommand(ProxyClientPrivate* client, ProtobufMessage::NetMessage *message)
{
    QByteArray ba = m_ConnectionHash[client]->m_binUUID;
    message->set_clientid(ba.data(), ba.size()); // we put the client's id into message

    quint32 nr = NotZeroNumGen::getMsgNr();
    message->set_messagenr(nr);
    m_ConnectionHash[client]->m_pNetClient->sendMessage(m_protobufWrapper.protobufToByteArray(*message));
    return nr;
}

void ProxyPrivate::handleReceiveMessage(std::shared_ptr<google::protobuf::Message> message)
{
    std::shared_ptr<ProtobufMessage::NetMessage> netMessage = std::static_pointer_cast<ProtobufMessage::NetMessage>(message);
    if(netMessage->has_clientid()) {
        QByteArray key(netMessage->clientid().data(), netMessage->clientid().size());
        auto iter = m_ClientHash.constFind(key);
        if (iter != m_ClientHash.constEnd())
            iter.value()->transmitAnswer(netMessage);
    }
    // ? todo error handling in case of unknown clientid ?
    else
        qWarning() << "No ClientID";
}

void ProxyPrivate::receiveTcpError(VeinTcp::TcpPeer *peer, QAbstractSocket::SocketError errorCode)
{
    QHashIterator<ProxyClientPrivate*, ProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        ProxyConnection *pC = it.value();
        if(pC->m_pNetClient == peer) { // we found a client that was connected to netclient
            ProxyClientPrivate* client = it.key();
            client->transmitError(errorCode); // so this client will forward error
        }
    }
}

void ProxyPrivate::registerConnection(VeinTcp::TcpPeer *peer)
{
    QHashIterator<ProxyClientPrivate*, ProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        ProxyConnection *pC = it.value();
        if(pC->m_pNetClient == peer) { // we found a client that tried to connected to netclient
            ProxyClientPrivate* client = it.key();
            client->transmitConnection();
        }
    }
}

void ProxyPrivate::registerDisConnection(VeinTcp::TcpPeer *peer)
{
    QHashIterator<ProxyClientPrivate*, ProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        ProxyConnection *pC = it.value();
        if(pC->m_pNetClient == peer) { // we found a client that tried to connected to netclient
            ProxyClientPrivate* client = it.key();
            client->transmitDisConnection(); // so this client will be forwarded connection
        }
    }
}

void ProxyPrivate::onMessageReceived(VeinTcp::TcpPeer *peer, const QByteArray &message)
{
    Q_UNUSED(peer)
    handleReceiveMessage(m_protobufWrapper.byteArrayToProtobuf(message));
}

ProxyNetPeer *ProxyPrivate::getProxyNetPeer(const QString &ipadress, quint16 port,
                                            VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory)
{
    ProxyNetPeer* netClient = searchConnection(ipadress, port);
    if(!netClient)  {// look for existing connection
        netClient = new ProxyNetPeer(tcpNetworkFactory, this);
        connect(netClient, &ProxyNetPeer::sigMessageReceived, this, &ProxyPrivate::onMessageReceived);
        connect(netClient, &ProxyNetPeer::sigSocketError, this, &ProxyPrivate::receiveTcpError);
        connect(netClient, &ProxyNetPeer::sigConnectionEstablished, this, &ProxyPrivate::registerConnection);
        connect(netClient, &ProxyNetPeer::sigConnectionClosed, this, &ProxyPrivate::registerDisConnection);
    }
    return netClient;
}

ProxyNetPeer* ProxyPrivate::searchConnection(const QString &ip, quint16 port)
{
    QHashIterator<ProxyClientPrivate*, ProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        ProxyConnection *pC = it.value();
        if( pC->m_sIP == ip && pC->m_nPort == port)
             return pC->m_pNetClient;
    }
    return nullptr;
}

}
