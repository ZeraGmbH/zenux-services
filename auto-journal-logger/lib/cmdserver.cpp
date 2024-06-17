#include "cmdserver.h"
#include "cmdparserbase.h"
#include <systemd/sd-daemon.h>

CmdServer::CmdServer(QObject* parent) : QObject(parent)
{
    m_pParser = nullptr;
    connect(&m_Server, &QTcpServer::newConnection, this, &CmdServer::OnClientNew);
}

bool CmdServer::Setup(quint16 ui16IPPort, QSimpleCmdParserSocketBase *pParser)
{
    bool retVal = false;
    m_pParser = pParser;
    pParser->SetListenPort(ui16IPPort);

    //check if systemd has handed us a socket (service socket activation)
    int tmpSocketDescriptor = -1;
    const int socketCount = sd_listen_fds(0);
    for(int i=0; i<socketCount; ++i) {
        if(sd_is_socket_inet(SD_LISTEN_FDS_START+i, AF_UNSPEC, SOCK_STREAM, 1, ui16IPPort)) {
            tmpSocketDescriptor = SD_LISTEN_FDS_START+i;
            break;
        }
    }
    if(tmpSocketDescriptor >= SD_LISTEN_FDS_START) {
        if(m_Server.setSocketDescriptor(tmpSocketDescriptor)) {
            retVal = true;
            qInfo()<<"Inherited socket descriptor from systemd, listening on port:" << ui16IPPort;
        }
    }
    if(!m_Server.isListening()) {
        if(m_Server.listen(QHostAddress::Any, ui16IPPort)) {
            retVal = true;
            qInfo()<<"Server Started on port:" << ui16IPPort;
        }
        else
            qCritical() << "Server could not listen on port:" << ui16IPPort << "error:" << m_Server.errorString();
    }
    return retVal;
}

void CmdServer::OnClientNew()
{
    QTcpSocket* pClientSocket = m_Server.nextPendingConnection();
    QSimpleCmdClient* pClient = new QSimpleCmdClient(this, pClientSocket, m_pParser);
    m_ClientHash[pClientSocket] = pClient;
    qInfo("Client connected to %s parser", qPrintable(m_pParser->GetParserName()));
    connect(pClientSocket, &QTcpSocket::disconnected, this, &CmdServer::OnClientDisconnect);
}

void CmdServer::OnClientDisconnect()
{
    QTcpSocket *pSocket = qobject_cast<QTcpSocket*>(sender());
    if(pSocket) {
        QSimpleCmdClient* pClient = m_ClientHash.take(pSocket);
        qInfo("Client disconnected from %s parser", qPrintable(m_pParser->GetParserName()));
        delete pClient;
        pSocket->deleteLater();
    }
}

