#ifndef INTERFACE_P_H
#define INTERFACE_P_H

#include <proxyclient.h>
#include <netmessages.pb.h>
#include <QObject>
#include <QAbstractSocket>
#include <QHash>
#include <QVariant>

namespace Zera {

class cInterfacePrivate: public QObject
{
    Q_OBJECT
public:
    static int getInstanceCount();
protected:
    cInterfacePrivate();
    ~cInterfacePrivate();
    struct TAnswerDecoded
    {
        quint32 msgNr;
        QString msgBody;
        int reply;
        int cmdSendEnumVal;
    };
    bool decodeProtobuffAnswer(const std::shared_ptr<ProtobufMessage::NetMessage> &message,
                               TAnswerDecoded &decodedAnswer,
                               int interruptEnumVal = -1);
    quint32 sendCommand(const QString &cmd);
    quint32 sendCommand(const QString &cmd, const QString &par);

    Zera::ProxyClientPtr m_clientSmart;
    QHash<quint32, int> m_MsgNrCmdList;

private:
    static int m_instanceCount;
};

}

#endif // INTERFACE_P_H
