#ifndef COMMONSCPIMETHODS_H
#define COMMONSCPIMETHODS_H

#include "protonetcommand.h"
#include <scpi.h>
#include <xiqnetwrapper.h>
#include <QTcpSocket>
#include <memory>

class CommonScpiMethods
{
public:
    static void sendProtoAnswer(QTcpSocket* telnetSocket,
                                XiQNetWrapper *protobufWrapper,
                                ProtonetCommandPtr protoCmd);
    static QString handleScpiInterfaceRead(std::shared_ptr<cSCPI> scpiInterface,
                                           const QString &scpiInput);
};

#endif // COMMONSCPIMETHODS_H
