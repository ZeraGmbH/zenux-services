#ifndef SINGLETONDEVICENODEPCBCTRL_H
#define SINGLETONDEVICENODEPCBCTRL_H

#include "abstractdevicenodepcbctrl.h"

class SingletonDeviceNodePcbCtrl
{
public:
    static AbstractDeviceNodePcbCtrlPtr getInstancePtr();
private:
    static AbstractDeviceNodePcbCtrlPtr m_instance;
};

#endif // SINGLETONDEVICENODEPCBCTRL_H
