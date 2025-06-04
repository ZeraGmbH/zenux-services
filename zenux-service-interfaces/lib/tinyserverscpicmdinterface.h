#ifndef TINYSERVERSCPICMDINTERFACE_H
#define TINYSERVERSCPICMDINTERFACE_H

#include "abstractserverInterface.h"
#include "proxyclient.h"

class TinyServerScpiCmdInterface : public AbstractServerInterface
{
    Q_OBJECT
public:
    TinyServerScpiCmdInterface(Zera::ProxyClientPtr client);
    virtual quint32 scpiCommand(const QString &scpi) override;

private:
    quint32 sendCommand(const QString &cmd);
    quint32 sendCommand(const QString &cmd, const QString &par);
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message);

    struct TAnswerDecoded
    {
        quint32 msgNr;
        QString msgBody;
        int reply;
        quint32 cmdSendEnumVal;
    };
    Zera::ProxyClientPtr m_clientSmart;
};

#endif // TINYSERVERSCPICMDINTERFACE_H
