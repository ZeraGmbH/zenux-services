#ifndef PROTOBUFSCPITESTCLIENT_H
#define PROTOBUFSCPITESTCLIENT_H

#include <QString>

class ProtobufScpiTestClient
{
public:
    static QString query(QString scpiQuery);
};

#endif // PROTOBUFSCPITESTCLIENT_H
