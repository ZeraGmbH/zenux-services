#include "ctrlheartbeatwait.h"
#include <QTimer>
#include <unistd.h>
#include <fcntl.h>

CtrlHeartbeatWait::CtrlHeartbeatWait(QString devNode) :
    m_sDeviceNode(devNode)
{
    m_TimerTO.setSingleShot(true);
    m_TimerTO.setInterval(10000);
    connect(&m_TimerTO, &QTimer::timeout, this, &CtrlHeartbeatWait::doTimeout);
    m_TimerPeriod.setSingleShot(false);
    m_TimerPeriod.setInterval(100);
}

void CtrlHeartbeatWait::start()
{
    qInfo("Atmel run-detection started");
    m_TimerTO.start();
    m_TimerPeriod.start();
    connect(&m_TimerPeriod, &QTimer::timeout, this, &CtrlHeartbeatWait::doAtmelTest);
}

void CtrlHeartbeatWait::doAtmelTest()
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
                    disconnect(&m_TimerTO, 0, this, 0);
                    disconnect(&m_TimerPeriod, 0, this, 0);
                    m_TimerTO.stop();
                    m_TimerPeriod.stop();
                    emit sigRunning();
                }
            }
        }
    }
}

void CtrlHeartbeatWait::doTimeout()
{
    qCritical("Atmel did not start within timeout");
    disconnect(&m_TimerPeriod, 0, this, 0);
    m_TimerPeriod.stop();
    emit sigTimeout();
}
