#ifndef I2CCTLBOOTLOADERSTOPPER_H
#define I2CCTLBOOTLOADERSTOPPER_H

#include "abstractfactoryi2cctrl.h"
#include <timerfactoryqt.h>

class I2cCtlBootloaderStopper : public QObject
{
    Q_OBJECT
public:
    I2cCtlBootloaderStopper(AbstractFactoryI2cCtrlPtr ctrlFactory, int channelId);
    void stopBootloader(int msWaitForApplicationStart = 1000);
signals:
    void sigAssumeBootloaderStopped(int channelId);

private slots:
    void onAppStartWaitFinished();
private:
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    int m_channelId;
    TimerTemplateQtPtr m_appStartTimer;
    bool m_bootloaderStopped = false;
};

typedef std::shared_ptr<I2cCtlBootloaderStopper> I2cCtlBootloaderStopperPtr;

#endif // I2CCTLBOOTLOADERSTOPPER_H
