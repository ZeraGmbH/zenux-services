#include "atmelctrlfactoryfortest.h"
#include "atmelemobctrlfortest.h"

void AtmelCtrlFactoryForTest::enableTest()
{
    m_emobCreateFunction = [](QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 channelMux, quint8 debuglevel)
    {
        ZeraMcontrollerBasePtr i2cCtrl = std::make_shared<ZeraMcontrollerBase>(devnode, adrCtrl, debuglevel);
        std::shared_ptr<AtmelEmobCtrlForTest> emobCtrl = std::make_shared<AtmelEmobCtrlForTest>(i2cCtrl, devnode, adrMux, channelMux);
        emobCtrl->setAdrCtrl(adrCtrl);
        emobCtrl->setDebuglevel(debuglevel);
        return emobCtrl;
    };
}
