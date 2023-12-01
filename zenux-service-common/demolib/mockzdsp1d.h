#ifndef MOCKZDSP1D_H
#define MOCKZDSP1D_H

#include "zdspserver.h"
#include "dspdevicenode.h"
#include <memory>

class MockZdsp1d : public ZDspServer
{
public:
    MockZdsp1d(int dspMagicId = DspDeviceNode::MAGIC_ID21362);
private:
};

#endif // MOCKZDSP1D_H
