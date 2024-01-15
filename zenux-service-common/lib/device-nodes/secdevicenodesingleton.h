#ifndef SECDEVICENODESINGLETON_H
#define SECDEVICENODESINGLETON_H

#include "secdevicenodeabstract.h"
#include <functional>

class SecDeviceNodeSingleton
{
public:
    static SecDeviceNodeAbstract *getInstance();
protected:
    static std::function<SecDeviceNodeAbstractPtr()> m_createFunction;
private:
    static SecDeviceNodeAbstractPtr m_instance;
};

#endif // SECDEVICENODESINGLETON_H
