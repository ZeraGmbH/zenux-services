#ifndef SECDEVICENODESINGLETON_H
#define SECDEVICENODESINGLETON_H

#include "abstractsecdevicenode.h"
#include <functional>

class SecDeviceNodeSingleton
{
public:
    static AbstractSecDeviceNode *getInstance();
protected:
    static std::function<AbstractSecDeviceNodePtr()> m_createFunction;
private:
    static AbstractSecDeviceNodePtr m_instance;
};

#endif // SECDEVICENODESINGLETON_H
