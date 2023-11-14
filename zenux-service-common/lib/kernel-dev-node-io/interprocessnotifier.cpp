#include "interprocessnotifier.h"
#include <unistd.h>
#include <fcntl.h>

static constexpr int readEndOfPipe = 0;
static constexpr int writeEndOfPipe = 1;

InterProcessNotifier::~InterProcessNotifier()
{
    close();
}

bool InterProcessNotifier::open()
{
    if(!m_notifier && m_pipeFileDescriptors[readEndOfPipe] == 0 && m_pipeFileDescriptors[writeEndOfPipe] == 0)
        if(pipe(m_pipeFileDescriptors) !=-1)
            if(fnctlPipe())
                return createNotifier();
    return false;
}

void InterProcessNotifier::close()
{
    if(m_notifier) {
        m_pipeFileDescriptors[readEndOfPipe] = ::close(m_pipeFileDescriptors[readEndOfPipe]);
        m_pipeFileDescriptors[writeEndOfPipe] = ::close(m_pipeFileDescriptors[writeEndOfPipe]);
        m_notifier.release();
    }
}

void InterProcessNotifier::sendSignal()
{
    ::write(m_pipeFileDescriptors[writeEndOfPipe], "I", 1);
}

void InterProcessNotifier::onSignal()
{
    char buf;
    read(m_pipeFileDescriptors[readEndOfPipe], &buf, 1);
    emit sigSignal();
}

bool InterProcessNotifier::fnctlPipe()
{
    return fcntl(m_pipeFileDescriptors[readEndOfPipe], F_SETFL, O_NONBLOCK) != -1 &&
           fcntl(m_pipeFileDescriptors[writeEndOfPipe], F_SETFL, O_NONBLOCK) != -1;
}

bool InterProcessNotifier::createNotifier()
{
    m_notifier = std::make_unique<QSocketNotifier>(m_pipeFileDescriptors[readEndOfPipe], QSocketNotifier::Read);
    connect(m_notifier.get(), &QSocketNotifier::activated,
            this, &InterProcessNotifier::onSignal);
    return m_notifier != nullptr;
}
