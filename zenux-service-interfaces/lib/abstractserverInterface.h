#ifndef ABSTRACTSERVERINTERFACE_H
#define ABSTRACTSERVERINTERFACE_H

#include "networkconnectioninfo.h"
#include "proxyclient.h"
#include <QAbstractSocket>
#include <QVariant>
#include <abstracttcpnetworkfactory.h>

[[maybe_unused]] constexpr int CONNECTION_TIMEOUT = 25000;
[[maybe_unused]] constexpr int TRANSACTION_TIMEOUT = 3000;

class AbstractServerInterface : public QObject
{
    Q_OBJECT
public:
    void setClientSuperSmart(const NetworkConnectionInfo &netInfo,
                             const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory);
    virtual void setClientSmart(const Zera::ProxyClientPtr &client) = 0;
    virtual const Zera::ProxyClientPtr &getClientSmart() = 0;

    virtual quint32 scpiCommand(const QString &scpi) = 0;
signals:
    void tcpError(QAbstractSocket::SocketError errorCode);
    void serverAnswer(quint32 msgnr, quint8 reply, const QVariant &answer);
};

typedef std::shared_ptr<AbstractServerInterface> AbstractServerInterfacePtr;

#endif // ABSTRACTSERVERINTERFACE_H
