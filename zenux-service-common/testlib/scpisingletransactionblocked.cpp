#include "scpisingletransactionblocked.h"
#include "proxy.h"
#include <timemachineobject.h>

QString ScpiSingleTransactionBlocked::query(const QString &scpiQuery,
                                            quint16 port,
                                            const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                                            const Zera::ProxyClientPtr &proxyClient)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(scpiQuery.toStdString());

    return sendBlocked(envelope, port, tcpNetworkFactory, proxyClient);
}

QString ScpiSingleTransactionBlocked::cmd(const QString &scpiCmd,
                                          const QString &param,
                                          quint16 port,
                                          const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                                          const Zera::ProxyClientPtr &proxyClient)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(scpiCmd.toStdString());
    // Oh no ';' how annoying sometimes needed sometimes not - see cmdXmlParam
    message->set_parameter(param.toStdString() + ";");

    return sendBlocked(envelope, port, tcpNetworkFactory, proxyClient);
}

QString ScpiSingleTransactionBlocked::cmdXmlParam(const QString &scpiCmd,
                                                  const QString &param,
                                                  quint16 port,
                                                  const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                                                  const Zera::ProxyClientPtr &proxyClient)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(scpiCmd.toStdString());
    message->set_parameter(param.toStdString());

    return sendBlocked(envelope, port, tcpNetworkFactory, proxyClient);
}

QString ScpiSingleTransactionBlocked::sendBlocked(ProtobufMessage::NetMessage &envelope,
                                                  quint16 port,
                                                  const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                                                  Zera::ProxyClientPtr proxyClient)
{
    if(!proxyClient)
        proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", port, tcpNetworkFactory);
    QString bareScpiAnswer;
    QObject::connect(proxyClient.get(), &Zera::ProxyClient::answerAvailable, [&](std::shared_ptr<ProtobufMessage::NetMessage> message) {
        bareScpiAnswer = QString::fromStdString(message->reply().body());
    });
    proxyClient->transmitCommand(&envelope);
    TimeMachineObject::feedEventLoop();
    return bareScpiAnswer;
}
