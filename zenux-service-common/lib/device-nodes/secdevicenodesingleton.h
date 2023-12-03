#ifndef SECDEVICENODESINGLETON_H
#define SECDEVICENODESINGLETON_H

#include "secdevicenodeinterface.h"
#include <functional>

class SecDeviceNodeSingleton
{
public:
    static SecDeviceNodeInterface *getInstance();
protected:
    static std::function<SecDeviceNodeInterfaceUPtr()> m_createFunction;
private:
    static SecDeviceNodeInterfaceUPtr m_instance;
};

#endif // SECDEVICENODESINGLETON_H
