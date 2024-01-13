#include "mockmt310s2dfull.h"
#include "mockpcbserver.h"

MockMt310s2dFull::MockMt310s2dFull() :
    cMT310S2dServer(MockPcbServer::createParams("mt310s2d"))
{
}
