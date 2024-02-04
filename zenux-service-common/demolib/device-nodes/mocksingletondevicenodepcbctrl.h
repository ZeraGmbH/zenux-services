#ifndef MOCKSINGLETONDEVICENODEPCBCTRL_H
#define MOCKSINGLETONDEVICENODEPCBCTRL_H

#include "abstractdevicenodepcbctrl.h"

class MockSingletonDeviceNodePcbCtrl
{
public:
    static AbstractDeviceNodePcbCtrlPtr getInstancePtr();
private:
    static AbstractDeviceNodePcbCtrlPtr m_instance;
};

#endif // MOCKSINGLETONDEVICENODEPCBCTRL_H
