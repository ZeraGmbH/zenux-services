#ifndef MOCKSECDEVICENODESINGLETON_H
#define MOCKSECDEVICENODESINGLETON_H

#include <secdevicenodesingleton.h>

class MockSecDeviceNodeSingleton : public SecDeviceNodeSingleton
{
public:
    static void enableMock();
};

#endif // MOCKSECDEVICENODESINGLETON_H
