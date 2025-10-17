#ifndef CONSOLESERVER_H
#define CONSOLESERVER_H

#include <QTcpServer>
#include <QTcpSocket>

class ConsoleServer : public QObject
{
    Q_OBJECT
public:
    ConsoleServer();
    void open(int portNo);
    QTcpSocket* getSocket();

signals:
    void sigLinesReceived(const QString &lines);

private slots:
    void onClientConnected();
    void onDataReceived();
    void onDisconnect();
private:
    QTcpServer* m_server = nullptr;
    QTcpSocket* m_socket = nullptr;
};

#endif // CONSOLESERVER_H
