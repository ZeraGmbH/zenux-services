#include "tasksourceswitchload.h"

TaskTemplatePtr TaskSourceSwitchLoad::create(const QJsonObject &loadPoint,
                                             const QJsonObject &jsonCapabilies,
                                             AbstractFactoryI2cCtrlPtr ctrlFactory,
                                             JsonDeviceStatusApi jsonSourceStateApi,
                                             std::shared_ptr<NotificationString> sourceState,
                                             NotificationString m_sourceLoadState)
{

}

TaskSourceSwitchLoad::TaskSourceSwitchLoad()
{
}
