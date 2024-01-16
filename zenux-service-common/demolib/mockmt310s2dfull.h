#ifndef MOCKMT310S2DFULL_H
#define MOCKMT310S2DFULL_H

#include "mt310s2d.h"

class MockMt310s2dFull
{
public:
    MockMt310s2dFull(QString alternateConfigXml = QString());
private:
    std::unique_ptr<cMT310S2dServer> m_server;
};

#endif // MOCKMT310S2DFULL_H
