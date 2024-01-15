#ifndef MOCKDSPDEVICENODESINGLETON_H
#define MOCKDSPDEVICENODESINGLETON_H

#include <dspdevicenodesingleton.h>
#include <dspdevicenode.h>

class MockDspDeviceNodeSingleton : public DspDeviceNodeSingleton
{
public:
    static void enableMock(int dspMagicId  = DspDeviceNode::MAGIC_ID21362);
};

#endif // MOCKDSPDEVICENODESINGLETON_H
