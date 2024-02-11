#ifndef RMCONNECTION_H
#define RMCONNECTION_H

#include <xiqnetwrapper.h>
#include <xiqnetpeer.h>
#include <netmessages.pb.h>
#include <QString>
#include <QAbstractSocket>

class RMConnection: public QObject
{
    Q_OBJECT
public:
    RMConnection(QString ipadr, quint16 port);
    virtual ~RMConnection();
    void connect2RM();
    void SendIdent(QString ident);
    void SendCommand(QString& cmd, QString &par, quint32 msgnr);
    void SendCommand(QString &cmd, QString &par);
private slots:
    void tcpErrorHandler(XiQNetPeer *peer, QAbstractSocket::SocketError errorCode);
    void onMessageReceived(XiQNetPeer *peer, QByteArray message);
private:
    void responseHandler(std::shared_ptr<google::protobuf::Message> response);
    QString m_sIPAdr;
    quint16 m_nPort;
    QString m_sCommand;
    XiQNetPeer* m_pResourceManagerClient = nullptr;
    XiQNetWrapper m_protobufWrapper;
signals:
    void connectionRMError();
    void connected();
    void rmAck(quint32);
};

#endif // RMCONNECTION_H
