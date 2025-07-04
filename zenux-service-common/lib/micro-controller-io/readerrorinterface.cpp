#include "readerrorinterface.h"
#include "i2cctrlreaderror.h"
#include "timerfactoryqt.h"

constexpr int READ_CONTROLLER_POLLING_PERIOD_MS = 2000;
constexpr qint8 MAX_DEVICES = 3;

ReadErrorInterface::ReadErrorInterface(AbstractFactoryI2cCtrlPtr ctrlFactory) :
    m_ctrlFactory(ctrlFactory)
{
    m_pollingTimer = TimerFactoryQt::createPeriodic(READ_CONTROLLER_POLLING_PERIOD_MS);
    connect(m_pollingTimer.get(), &TimerTemplateQt::sigExpired, this, &ReadErrorInterface::printControllerErrorToLog);
    m_pollingTimer->start();
}


void ReadErrorInterface::printControllerErrorToLog(void)
{
    QString ErrReply;
    switch (currentDevice) {
    case 0:
        m_ctrlFactory->getErrorlogController()->readErrorSystemCtrl(ErrReply);
        if(ErrReply.length() > 1)
            qWarning("Error System-controller: %s", qPrintable(ErrReply));
        else
            qInfo("Error System-controller: none");
        break;

    case 1:
        m_ctrlFactory->getErrorlogController()->readErrorRelayCtrl(ErrReply);
        if(ErrReply.length() > 1)
            qWarning("Error Relay-controller: %s", qPrintable(ErrReply));
        else
            qInfo("Error Relay-controller: none");
        break;

    case 2:
        m_ctrlFactory->getErrorlogController()->readErrorEmobCtrl(ErrReply);
        if(ErrReply.length() > 1)
            qWarning("Error Emob-controller: %s", qPrintable(ErrReply));
        else
            qInfo("Error Emob-controller: none");
        break;

    default:
        qWarning("Unvalid value currentDevice");
        break;
    }
    currentDevice++;
    if(currentDevice >= MAX_DEVICES)
        currentDevice = 0;
}
