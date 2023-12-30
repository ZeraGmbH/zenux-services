#include "protobufscpitestclient.h"
#include "proxy.h"
#include <timemachineobject.h>

QString ProtobufScpiTestClient::query(QString scpiQuery)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(scpiQuery.toStdString());
    Zera::ProxyClientPtr proxylient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    QString bareScpiAnswer;
    QObject::connect(proxylient.get(), &Zera::ProxyClient::answerAvailable, [&](std::shared_ptr<ProtobufMessage::NetMessage> message) {
        bareScpiAnswer = QString::fromStdString(message->reply().body());
    });
    proxylient->transmitCommand(&envelope);
    TimeMachineObject::feedEventLoop();
    return bareScpiAnswer;
}

QString ProtobufScpiTestClient::cmd(QString scpiCmd, QString param)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();
    message->set_command(scpiCmd.toStdString());
    message->set_parameter(param.toStdString() + ";"); // Oh no ';' how annoying sometimes needed sometimes not
    Zera::ProxyClientPtr proxylient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    QString bareScpiAnswer;
    QObject::connect(proxylient.get(), &Zera::ProxyClient::answerAvailable, [&](std::shared_ptr<ProtobufMessage::NetMessage> message) {
        bareScpiAnswer = QString::fromStdString(message->reply().body());
    });
    proxylient->transmitCommand(&envelope);
    TimeMachineObject::feedEventLoop();
    return bareScpiAnswer;
}
