#ifndef ABSTRACTZERAMCONTROLLERDELAYED_H
#define ABSTRACTZERAMCONTROLLERDELAYED_H

#include <zeramcontrolleriotemplate.h>
#include <QObject>

class AbstractZeraMControllerDelayed : public QObject
{
    Q_OBJECT
signals:
    void sigCmdDone(ZeraMControllerIoTemplate::atmelRM cmdResult);
};

typedef std::unique_ptr<AbstractZeraMControllerDelayed> AbstractZeraMControllerDelayedPtr;

#endif // ABSTRACTZERAMCONTROLLERDELAYED_H
