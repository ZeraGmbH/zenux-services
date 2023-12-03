#include "mocksec1000d.h"
#include "mockpcbserver.h"
#include "secdevicenodesingletonmock.h"

MockSec1000d::MockSec1000d() :
    cSEC1000dServer(MockPcbServer::createParams("sec1000d"))
{
    SecDeviceNodeSingletonMock::enableMock();
}
