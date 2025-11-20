#include "i2cctlbootloaderstopper.h"

I2cCtlBootloaderStopper::I2cCtlBootloaderStopper(AbstractFactoryI2cCtrlPtr ctrlFactory, int channelId) :
    m_ctrlFactory(ctrlFactory),
    m_channelId(channelId)
{
}

void I2cCtlBootloaderStopper::stopBootloader(int msWaitForApplicationStart)
{
    if(!m_bootloaderStopped && !m_appStartTimer) {
        ZeraMControllerIoTemplate::atmelRM result = m_ctrlFactory->getBootloaderController()->bootloaderStartProgram();
        if(result == ZeraMControllerIoTemplate::cmddone) {
            qInfo("Stopping bootloader succeeded - wait %fs for app startup...", float(msWaitForApplicationStart/1000));
            m_appStartTimer = TimerFactoryQt::createSingleShot(msWaitForApplicationStart);
            connect(m_appStartTimer.get(), &TimerTemplateQt::sigExpired,
                    this, &I2cCtlBootloaderStopper::onAppStartWaitFinished);
            m_appStartTimer->start();
        }
        else {
            qInfo("Stopping bootloader failed. Either application running or no controller connected.");
            m_bootloaderStopped = true;
            emit sigAssumeBootloaderStopped(m_channelId);
        }
    }
}

void I2cCtlBootloaderStopper::onAppStartWaitFinished()
{
    qInfo("Waiting for µC-application start finished.");
    m_bootloaderStopped = true;
    emit sigAssumeBootloaderStopped(m_channelId);
}
