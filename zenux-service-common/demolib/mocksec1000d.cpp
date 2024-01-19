#include "mocksec1000d.h"
#include "serverparamgenerator.h"
#include "mocksecdevicenodesingleton.h"

MockSec1000d::MockSec1000d() :
    cSEC1000dServer(ServerParamGenerator::createParams("sec1000d"))
{
    MockSecDeviceNodeSingleton::enableMock();
}
