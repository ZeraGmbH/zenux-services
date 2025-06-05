#ifndef TINYZSCPICMDINTERFACE_H
#define TINYZSCPICMDINTERFACE_H

#include "abstractserverInterface.h"
#include "proxyclient.h"

class TinyZScpiCmdInterface : public AbstractServerInterface
{
    Q_OBJECT
public:
    TinyZScpiCmdInterface(Zera::ProxyClientPtr client);
    virtual quint32 scpiCommand(const QString &scpi) override;

private:
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

#endif // TINYZSCPICMDINTERFACE_H
