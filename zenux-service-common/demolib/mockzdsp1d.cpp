#include "mockzdsp1d.h"
#include "mockpcbserver.h"
#include "dspdevicenodemock.h"

MockZdsp1d::MockZdsp1d(int dspMagicId) :
    ZDspServer(std::make_unique<DspDeviceNodeMock>(dspMagicId), MockPcbServer::createParams("zdsp1d"))
{
}
