#ifndef RMCONNECTION_H
#define RMCONNECTION_H

#include <xiqnetwrapper.h>
#include <xiqnetpeer.h>
#include <netmessages.pb.h>
#include <QString>
#include <QAbstractSocket>

class cRMConnection: public QObject
{
    Q_OBJECT
public:
    cRMConnection(QString ipadr, quint16 port);
    void connect2RM();
    void SendIdent(QString ident);
public slots:
    void SendCommand(QString& cmd, QString &par);
private:
    QString m_sIPAdr;
    quint16 m_nPort;
    QString m_sCommand;
    XiQNetPeer* m_pResourceManagerClient;
    XiQNetWrapper m_ProtobufWrapper;
private slots:
    void tcpErrorHandler(QAbstractSocket::SocketError errorCode);
    void responseHandler(std::shared_ptr<google::protobuf::Message> message);
signals:
    void connectionRMError();
    void connected();
};


#endif // RMCONNECTION_H
