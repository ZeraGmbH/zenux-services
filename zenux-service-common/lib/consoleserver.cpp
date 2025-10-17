#include "consoleserver.h"

ConsoleServer::ConsoleServer()
{
}

void ConsoleServer::open(int portNo)
{
    m_server = new QTcpServer(this);
    m_server->setMaxPendingConnections(1); // we only accept 1 client to connect
    connect(m_server, &QTcpServer::newConnection, this, &ConsoleServer::onClientConnected);
    m_server->listen(QHostAddress::AnyIPv4, portNo);
}

QTcpSocket *ConsoleServer::getSocket()
{
    return m_socket;
}

void ConsoleServer::onClientConnected()
{
    qInfo("External SCPI Client connected");
    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QIODevice::readyRead, this, &ConsoleServer::onDataReceived);
    connect(m_socket, &QAbstractSocket::disconnected, this, &ConsoleServer::onDisconnect);
}

void ConsoleServer::onDataReceived()
{
    QString input;
    while ( m_socket->canReadLine() )
        input += m_socket->readLine();
    input.remove('\r'); // we remove cr lf
    input.remove('\n');
    qInfo("External SCPI command: %s", qPrintable(input));
    emit sigLinesReceived(input);
}

void ConsoleServer::onDisconnect()
{
    qInfo("External SCPI Client disconnected");
    disconnect(m_socket, 0, 0, 0); // we disconnect everything
}
