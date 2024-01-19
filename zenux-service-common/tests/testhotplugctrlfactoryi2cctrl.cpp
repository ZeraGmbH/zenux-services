#include "testhotplugctrlfactoryi2cctrl.h"
#include "testhotplugi2cctrlcommoninfo.h"

TestHotPlugCtrlFactoryI2cCtrl::TestHotPlugCtrlFactoryI2cCtrl(I2cSettings *i2cSettings) :
    m_i2cSettings(i2cSettings)
{
}

I2cCtrlCommonInfoPtrUnique TestHotPlugCtrlFactoryI2cCtrl::getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(muxChannel)
    Q_ASSERT(ctrlType == AbstractFactoryI2cCtrl::CTRL_TYPE_EMOB);
    bool repondingCtrl = true;
    if(!m_respondOnVersionList.isEmpty())
        repondingCtrl = m_respondOnVersionList.takeLast();
    return std::make_unique<TestHotplugI2cCtrlCommonInfo>(m_i2cSettings, muxChannel, repondingCtrl, m_ctrlInstanceCount);
}

int TestHotPlugCtrlFactoryI2cCtrl::getCtrlInstanceCount()
{
    return m_ctrlInstanceCount;
}

void TestHotPlugCtrlFactoryI2cCtrl::prepareNextTestControllers(QVector<bool> respondOnVersionList)
{
    m_respondOnVersionList = respondOnVersionList;
}
