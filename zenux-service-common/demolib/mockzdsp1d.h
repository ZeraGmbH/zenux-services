#ifndef MOCKZDSP1D_H
#define MOCKZDSP1D_H

#include "zdspserver.h"

class MockZdsp1d
{
public:
    MockZdsp1d(AbstractFactoryDeviceNodeDspPtr deviceNodeFactory, QString alternateConfigXml = QString());
private:
    std::unique_ptr<ZDspServer> m_server;
};

#endif // MOCKZDSP1D_H
