#ifndef TASKSOURCESWITCHLOAD_H
#define TASKSOURCESWITCHLOAD_H

#include "abstractfactoryi2cctrl.h"
#include "jsondevicestatusapi.h"
#include "notificationstring.h"
#include <tasktemplate.h>

class TaskSourceSwitchLoad : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(const QJsonObject &loadPoint,
                                  const QJsonObject &jsonCapabilies,
                                  AbstractFactoryI2cCtrlPtr ctrlFactory,
                                  // We need notifiable source state / loadpoint
                                  // or signals here (not a task??)
                                  JsonDeviceStatusApi jsonSourceStateApi,
                                  std::shared_ptr<NotificationString> sourceState,
                                  NotificationString m_sourceLoadState
                                  );
    TaskSourceSwitchLoad();
};

#endif // TASKSOURCESWITCHLOAD_H
