#ifndef DELAYEDZERAMCONTROLLERTASK_H
#define DELAYEDZERAMCONTROLLERTASK_H

#include "abstractzeramcontrollerdelayed.h"
#include "protonetcommand.h"
#include <tasktemplate.h>

class DelayedZeraMControllerTask : public TaskTemplate
{
    Q_OBJECT
public:
    static std::unique_ptr<DelayedZeraMControllerTask> create(AbstractZeraMControllerDelayedPtr mcontroller,
                                                              ProtonetCommandPtr protoCmd,
                                                              const QString &rangeName);
    DelayedZeraMControllerTask(AbstractZeraMControllerDelayedPtr mcontroller,
                               ProtonetCommandPtr protoCmd,
                               const QString &rangeName);
    void start() override;
signals:
    void sigCmdDone(ProtonetCommandPtr protoCmd, QString rangeName);
private slots:
    void onCmdDone(ZeraMControllerIoTemplate::atmelRM cmdResult);
private:
    AbstractZeraMControllerDelayedPtr m_mcontroller;
    ProtonetCommandPtr m_protoCmd;
    QString m_rangeName;
};

#endif // DELAYEDZERAMCONTROLLERTASK_H
