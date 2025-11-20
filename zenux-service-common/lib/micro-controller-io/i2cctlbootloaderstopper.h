#ifndef I2CCTLBOOTLOADERSTOPPER_H
#define I2CCTLBOOTLOADERSTOPPER_H

#include "abstractfactoryi2cctrl.h"
#include <timerfactoryqt.h>

class I2cCtlBootloaderStopper : public QObject
{
    Q_OBJECT
public:
    I2cCtlBootloaderStopper(AbstractFactoryI2cCtrlPtr ctrlFactory,
                            int channelId,
                            quint8 muxChannel = -1,
                            AbstractFactoryI2cCtrl::ControllerTypes ctrlType = AbstractFactoryI2cCtrl::CTRL_TYPE_EMOB);
    void stopBootloader(int msWaitForApplicationStart = 1000);
signals:
    void sigAssumeBootloaderStopped(int channelId);

private slots:
    void onAppStartWaitFinished();
private:
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    int m_channelId;
    AbstractFactoryI2cCtrl::ControllerTypes m_ctrlType;
    quint8 m_muxChannel;
    TimerTemplateQtPtr m_appStartTimer;
    bool m_bootloaderStopped = false;
};

typedef std::shared_ptr<I2cCtlBootloaderStopper> I2cCtlBootloaderStopperPtr;

#endif // I2CCTLBOOTLOADERSTOPPER_H
