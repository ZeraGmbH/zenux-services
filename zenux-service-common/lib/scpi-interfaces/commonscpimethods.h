#ifndef COMMONSCPIMETHODS_H
#define COMMONSCPIMETHODS_H

#include "protonetcommand.h"
#include "sensesettings.h"
#include <scpi.h>
#include <xiqnetwrapper.h>
#include <QTcpSocket>
#include <memory>

class CommonScpiMethods
{
public:
    static void sendProtoAnswer(QTcpSocket* telnetSocket,
                                ProtonetCommandPtr protoCmd);
    static QString handleScpiInterfaceRead(std::shared_ptr<cSCPI> scpiInterface,
                                           const QString &scpiInput);
    static bool containsValidChannelMName(cSenseSettingsPtr senseSettings, const QString &channelMName);
    static bool containsValidChannelMNames(cSenseSettingsPtr senseSettings, const QStringList &channelMNames);
};

#endif // COMMONSCPIMETHODS_H
