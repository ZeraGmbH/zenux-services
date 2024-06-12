#include "cmdserver.h"
#include "cmdparserbase.h"

CmdServer::CmdServer(QObject* parent) : QObject(parent)
{
    m_pParser = nullptr;
    connect(&m_Server, &QTcpServer::newConnection, this, &CmdServer::OnClientNew);
}

bool CmdServer::Setup(quint16 ui16IPPort, QSimpleCmdParserSocketBase *pParser)
{
    m_pParser = pParser;
    pParser->SetListenPort(ui16IPPort);
    return m_Server.listen(QHostAddress::Any, ui16IPPort);
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

