#include "fasynchandler.h"
#include <QSet>
#include <unistd.h>
#include <fcntl.h>

static constexpr int readEndOfPipe = 0;
static constexpr int writeEndOfPipe = 1;

static int pipeFD[2];
static bool signalHandlerEstablished = false;

FAsyncHandler::FAsyncHandler()
{
}

FAsyncHandler::~FAsyncHandler()
{
}

void sigHandler(int)
{
    write(pipeFD[writeEndOfPipe], "I", 1);
}

void FAsyncHandler::setupSigHandler(int deviceFileDescriptor)
{
    if(!signalHandlerEstablished) {
        pipe(pipeFD);

        struct sigaction sigAction;
        sigAction.sa_handler = &sigHandler;
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


