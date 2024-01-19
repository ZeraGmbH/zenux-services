#include "hotplugctrltestfactory.h"
#include "hotplugctrltest.h"

HotPlugCtrlTestFactory::HotPlugCtrlTestFactory(I2cSettings *i2cSettings) :
    m_i2cSettings(i2cSettings)
{
}

I2cCtrlCommonVersionsPtrUnique HotPlugCtrlTestFactory::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    Q_ASSERT(ctrlType == AbstractFactoryI2cCtrl::CTRL_TYPE_EMOB);
    bool repondingCtrl = true;
    if(!m_respondOnVersionList.isEmpty())
        repondingCtrl = m_respondOnVersionList.takeLast();
    return std::make_unique<HotPlugCtrlTest>(m_i2cSettings, muxChannel, repondingCtrl, m_ctrlInstanceCount);
}

int HotPlugCtrlTestFactory::getCtrlInstanceCount()
{
    return m_ctrlInstanceCount;
}

void HotPlugCtrlTestFactory::prepareNextTestControllers(QVector<bool> respondOnVersionList)
{
    m_respondOnVersionList = respondOnVersionList;
}
