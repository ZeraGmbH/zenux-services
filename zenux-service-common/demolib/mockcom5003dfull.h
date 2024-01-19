#ifndef MOCKCOM5003DFULL_H
#define MOCKCOM5003DFULL_H

#include "com5003d.h"

class MockCom5003dFull
{
public:
    MockCom5003dFull(FactoryControllerAbstractPtr ctrlFactory,
                     QString alternateConfigXml = QString());
private:
    std::unique_ptr<cCOM5003dServer> m_server;
};

#endif // MOCKCOM5003DFULL_H
