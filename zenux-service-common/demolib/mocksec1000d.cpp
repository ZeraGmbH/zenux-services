#include "mocksec1000d.h"
#include "mockpcbserver.h"
#include "mocksecdevicenodesingleton.h"

MockSec1000d::MockSec1000d() :
    cSEC1000dServer(MockPcbServer::createParams("sec1000d"))
{
    MockSecDeviceNodeSingleton::enableMock();
}
