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
                                const ProtonetCommandPtr &protoCmd);
    static QString handleScpiInterfaceRead(const std::shared_ptr<cSCPI> &scpiInterface,
                                           const QString &scpiInput);
    static bool containsValidChannelMName(const cSenseSettingsPtr &senseSettings, const QString &channelMName);
    static bool containsValidChannelMNames(const cSenseSettingsPtr &senseSettings, const QStringList &channelMNames);
};

#endif // COMMONSCPIMETHODS_H
