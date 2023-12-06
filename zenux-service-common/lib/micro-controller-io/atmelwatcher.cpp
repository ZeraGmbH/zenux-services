#include "atmelwatcher.h"
#include <QTimer>
#include <unistd.h>
#include <fcntl.h>

cAtmelWatcher::cAtmelWatcher(QString devNode) :
    m_sDeviceNode(devNode)
{
    m_TimerTO.setSingleShot(true);
    m_TimerTO.setInterval(10000);
    connect(&m_TimerTO, &QTimer::timeout, this, &cAtmelWatcher::doTimeout);
    m_TimerPeriod.setSingleShot(false);
    m_TimerPeriod.setInterval(100);
}

void cAtmelWatcher::start()
{
    qInfo("Atmel run-detection started");
    m_TimerTO.start();
    m_TimerPeriod.start();
    connect(&m_TimerPeriod, &QTimer::timeout, this, &cAtmelWatcher::doAtmelTest);
}

void cAtmelWatcher::doAtmelTest()
{
    QByteArray ba = m_sDeviceNode.toLatin1();
    int ret = (fd = open(ba.data(),O_RDWR));
    if ( ret < 0 ) {
        qCritical("error %d opening fpga device: '%s'", ret, ba.data());
    }
    else
    {
        if (lseek(fd,0xffc,0) < 0 ) {
            close(fd);
            qCritical("error positioning fpga device: %s", ba.data());
        }
        else {
            long pcbTestReg;
            ret = read(fd,(char*) &pcbTestReg,4);
            close(fd);
            if (ret < 0 ) {
                qCritical("error reading fpga device: %s",ba.data());
            }
            else
            {
                if ((pcbTestReg & 1) > 0) {
                    qInfo("Atmel running");
                    m_TimerTO.disconnect(SIGNAL(timeout()));
                    m_TimerPeriod.disconnect(SIGNAL(timeout()));
                    m_TimerTO.stop();
                    m_TimerPeriod.stop();
                    emit running();
                }
            }
        }
    }
}

void cAtmelWatcher::doTimeout()
{
    qCritical("Atmel did not start within timeout");
    m_TimerPeriod.disconnect(SIGNAL(timeout()));
    m_TimerPeriod.stop();
    emit timeout();
}
