#include "emobctrlfactoryfortest.h"
#include "atmelctrlemobfortest.h"

QVector<bool> EmobCtrlFactoryForTest::m_respondingControllers;

void EmobCtrlFactoryForTest::prepareNextTestControllers(QVector<bool> responding)
{
    m_respondingControllers = responding;
    m_emobCreateFunction = [&](QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 channelMux, quint8 debuglevel)
    {
        bool controllerResponding = false;
        if(!m_respondingControllers.isEmpty())
            controllerResponding = m_respondingControllers.takeLast();
        ZeraMcontrollerIoPtr i2cCtrl = std::make_shared<ZeraMControllerIo>(devnode, adrCtrl, debuglevel);
        std::shared_ptr<AtmelCtrlEmobForTest> emobCtrl = std::make_shared<AtmelCtrlEmobForTest>(i2cCtrl, devnode, adrMux, channelMux, controllerResponding);
        emobCtrl->setAdrCtrl(adrCtrl);
        emobCtrl->setDebuglevel(debuglevel);
        return emobCtrl;
    };
}
