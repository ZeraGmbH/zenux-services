#ifndef CMDSERVER_H
#define CMDSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "cmdclient.h"

class QSimpleCmdParserSocketBase;

class CmdServer: public QObject
{
    Q_OBJECT
public:
    CmdServer(QObject *parent = nullptr);
    bool Setup(quint16 ui16IPPort, QSimpleCmdParserSocketBase *pParser);

public slots:
    void OnClientNew();
    void OnClientDisconnect();

private:
    QTcpServer m_Server;
    QHash<QTcpSocket*, QSimpleCmdClient*> m_ClientHash;
    QSimpleCmdParserSocketBase *m_pParser;
};

#endif // CMDSERVER_H
