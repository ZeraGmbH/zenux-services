#ifndef READERRORINTERFACE_H
#define READERRORINTERFACE_H

#include "timertemplateqt.h"
#include "abstractfactoryi2cctrl.h"


class ReadErrorInterface : public QObject
{
    Q_OBJECT
public:
    ReadErrorInterface(AbstractFactoryI2cCtrlPtr ctrlFactory);

private:
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    TimerTemplateQtPtr m_pollingTimer;
    qint8 currentDevice = 0;

public slots:
    void printControllerErrorToLog(void);

};

#endif // READERRORINTERFACE_H
