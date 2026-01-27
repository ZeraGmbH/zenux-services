#include "ctrlheartbeatwait.h"
#include <timerfactoryqt.h>
#include <unistd.h>
#include <fcntl.h>

CtrlHeartbeatWait::CtrlHeartbeatWait(const QString &devNode) :
    m_sDeviceNode(devNode)
{
}

void CtrlHeartbeatWait::start()
{
    qInfo("Atmel run-detection started");
    m_TimerTO = TimerFactoryQt::createSingleShot(10000);
    connect(m_TimerTO.get(), &TimerTemplateQt::sigExpired,
            this, &CtrlHeartbeatWait::doTimeout);
    m_TimerPeriod = TimerFactoryQt::createPeriodic(100);
    connect(m_TimerPeriod.get(), &TimerTemplateQt::sigExpired,
            this, &CtrlHeartbeatWait::doAtmelTest);
    m_TimerTO->start();
    m_TimerPeriod->start();
}

void CtrlHeartbeatWait::doAtmelTest()
{
    QByteArray ba = m_sDeviceNode.toLatin1();
    int fd = open(ba.data(), O_RDWR);
    if ( fd < 0 ) {
        qCritical("Error %d opening fpga device: '%s'", fd, ba.data());
        return;
    }
    if (lseek(fd,0xffc,0) < 0 ) {
        close(fd);
        qCritical("Error positioning fpga device: %s", ba.data());
        return;
    }

    long pcbTestReg;
    int ret = read(fd, (char*)&pcbTestReg, 4);
    close(fd);
    if (ret < 0 ) {
        qCritical("Error reading fpga device: %s",ba.data());
        return;
    }

    if ((pcbTestReg & 1) > 0) {
        qInfo("Atmel running");
        stopTimers();
        emit sigRunning();
    }
}

void CtrlHeartbeatWait::stopTimers()
{
    m_TimerTO.reset();
    m_TimerPeriod.reset();
}

void CtrlHeartbeatWait::doTimeout()
{
    qCritical("Atmel did not start within timeout");
    stopTimers();
    emit sigTimeout();
}
