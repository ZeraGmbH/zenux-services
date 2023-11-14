#ifndef INTERPROCESSPIPE_H
#define INTERPROCESSPIPE_H

#include <QSocketNotifier>
#include <memory>

class InterProcessNotifier : public QObject
{
    Q_OBJECT
public:
    ~InterProcessNotifier();
    bool open();
    void close();
    void sendSignal();
signals:
    void sigSignal();

private slots:
    void onSignal();
private:
    bool createNotifier();
    bool fnctlPipe();
    static constexpr int pipeEndCount = 2;
    int m_pipeFileDescriptors[pipeEndCount] = { 0 };
    std::unique_ptr<QSocketNotifier> m_notifier;
};

#endif // INTERPROCESSPIPE_H
