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
        if(m_ctrlFactory->getErrorlogController()->readErrorSystemCtrl(ErrReply) == ZeraMControllerIo::atmelRM::cmddone) {
            if(ErrReply.length() > 1)
                qWarning("Error System-controller: %s", qPrintable(ErrReply));
            else
                qInfo("Error System-controller: none");
        }
        else
            qWarning("Reading failed 'Error System-controller'");
        break;

    case 1:
        if(m_ctrlFactory->getErrorlogController()->readErrorRelayCtrl(ErrReply) == ZeraMControllerIo::atmelRM::cmddone) {
            if(ErrReply.length() > 1)
                qWarning("Error Relay-controller: %s", qPrintable(ErrReply));
            else
                qInfo("Error Relay-controller: none");
        }
        else
            qWarning("Reading failed 'Error Relay-controller'");
        break;

    case 2:
        if(m_ctrlFactory->getErrorlogController()->readErrorEmobCtrl(ErrReply) == ZeraMControllerIo::atmelRM::cmddone) {
            if(ErrReply.length() > 1)
                qWarning("Error Emob-controller: %s", qPrintable(ErrReply));
            else
                qInfo("Error Emob-controller: none");
        }
        else
            qWarning("Reading failed 'Error Emob-controller'");
        break;

    default:
        qWarning("Unvalid value 'currentDevice'");
        break;
    }
    currentDevice++;
    if(currentDevice >= MAX_DEVICES)
        currentDevice = 0;
}
