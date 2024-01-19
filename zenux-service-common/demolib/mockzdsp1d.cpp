#include "mockzdsp1d.h"
#include "serverparamgenerator.h"
#include "mockdspdevicenodesingleton.h"

MockZdsp1d::MockZdsp1d(int dspMagicId) :
    ZDspServer(ServerParamGenerator::createParams("zdsp1d"))
{
    MockDspDeviceNodeSingleton::enableMock(dspMagicId);
}
