#ifndef DSPDEVICENODESINGLETONMOCK_H
#define DSPDEVICENODESINGLETONMOCK_H

#include <dspdevicenodesingleton.h>
#include <dspdevicenode.h>

class DspDeviceNodeSingletonMock : public DspDeviceNodeSingleton
{
public:
    static void enableMock(int dspMagicId  = DspDeviceNode::MAGIC_ID21362);
};

#endif // DSPDEVICENODESINGLETONMOCK_H
