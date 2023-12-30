#ifndef PROTOBUFSCPITESTCLIENT_H
#define PROTOBUFSCPITESTCLIENT_H

#include <QString>

class ProtobufScpiTestClient
{
public:
    static QString query(QString scpiQuery);
    static QString cmd(QString scpiCmd, QString param);
};

#endif // PROTOBUFSCPITESTCLIENT_H
