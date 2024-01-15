#include "mockzdsp1d.h"
#include "mockpcbserver.h"
#include "mockdspdevicenodesingleton.h"

MockZdsp1d::MockZdsp1d(int dspMagicId) :
    ZDspServer(MockPcbServer::createParams("zdsp1d"))
{
    MockDspDeviceNodeSingleton::enableMock(dspMagicId);
}
