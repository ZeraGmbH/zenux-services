#ifndef FASYNCHANDLER_H
#define FASYNCHANDLER_H

#include <QSocketNotifier>
#include <csignal>

class FAsyncHandler
{
public:
    FAsyncHandler();
    virtual ~FAsyncHandler();
    QSocketNotifier *getNotifier(int deviceFileDescriptor);
private:
    static void setupSigHandler(int deviceFileDescriptor);
    QSocketNotifier *m_socketNotifier = nullptr;
};

#endif // FASYNCHANDLER_H
