#include "fasynchandler.h"
#include <QSet>
#include <functional>
#include <unistd.h>
#include <fcntl.h>

static constexpr int readEndOfPipe = 0;
static constexpr int writeEndOfPipe = 1;

//static int pipeFD[2];
static bool signalHandlerEstablished = false;

static constexpr int maxNotifiers = 8;

struct TInterProcessPipe
{
    int pipeFileDescriptors[2];
};
TInterProcessPipe pipes[maxNotifiers];


FAsyncHandler::FAsyncHandler()
{
}

template<int A>
void tsigHandler(int)
{
    write(pipes[A].pipeFileDescriptors[writeEndOfPipe], "I", 1);
}

void FAsyncHandler::setupSigHandler(int deviceFileDescriptor)
{
    if(!signalHandlerEstablished) {
        // TODO establish pipe

        struct sigaction sigAction;
        sigAction.sa_handler = &tsigHandler<0>;
        sigemptyset(&sigAction.sa_mask);
        sigAction.sa_flags = SA_RESTART;
        sigAction.sa_restorer = nullptr;
        sigaction(SIGIO, &sigAction, nullptr);

        fcntl(deviceFileDescriptor, F_SETOWN, getpid()); // make us owner - necessary / move ?
        int oflags = fcntl(deviceFileDescriptor, F_GETFL);
        fcntl(deviceFileDescriptor, F_SETFL, oflags | FASYNC);
        signalHandlerEstablished = true;
    }
}

QSocketNotifier *FAsyncHandler::getNotifier(int deviceFileDescriptor)
{
    setupSigHandler(deviceFileDescriptor);

}


