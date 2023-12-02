#include "mockzdsp1d.h"
#include "mockpcbserver.h"
#include "dspdevicenodesingletonmock.h"

MockZdsp1d::MockZdsp1d(int dspMagicId) :
    ZDspServer(MockPcbServer::createParams("zdsp1d"))
{
    DspDeviceNodeSingletonMock::enableMock(dspMagicId);
}
