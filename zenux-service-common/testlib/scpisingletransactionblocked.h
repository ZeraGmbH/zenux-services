#ifndef SCPISINGLETRANSACTIONBLOCKED_H
#define SCPISINGLETRANSACTIONBLOCKED_H

#include "netmessages.pb.h"
#include <QString>

class ScpiSingleTransactionBlocked
{
public:
    static QString query(QString scpiQuery);
    static QString cmd(QString scpiCmd, QString param);
    static QString cmdXmlParam(QString scpiCmd, QString param);
private:
    static QString sendBlocked(ProtobufMessage::NetMessage &envelope);
};

#endif // SCPISINGLETRANSACTIONBLOCKED_H
