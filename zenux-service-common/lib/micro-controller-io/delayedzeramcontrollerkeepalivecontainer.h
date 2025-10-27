#ifndef DELAYEDZERAMCONTROLLERKEEPALIVECONTAINER_H
#define DELAYEDZERAMCONTROLLERKEEPALIVECONTAINER_H

#include "abstractzeramcontrollerdelayed.h"
#include "protonetcommand.h"
#include <taskcontainersequence.h>

class DelayedZeraMControllerKeepAliveContainer : public QObject
{
    Q_OBJECT
public:
    ~DelayedZeraMControllerKeepAliveContainer() = default;
    void addPendingIo(AbstractZeraMControllerDelayedPtr mcontroller,
                      ProtonetCommandPtr protoCmd,
                      const QString &rangeName);
signals:
    void sigCmdDone(ProtonetCommandPtr protoCmd, QString rangeName);
private:
    TaskContainerSequence m_pendingTaskContainer;
};

#endif // DELAYEDZERAMCONTROLLERKEEPALIVECONTAINER_H
