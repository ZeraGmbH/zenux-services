#ifndef MOCKZDSP1D_H
#define MOCKZDSP1D_H

#include "zdspserver.h"
#include "dspdevicenode.h"

class MockZdsp1d
{
public:
    MockZdsp1d(QString alternateConfigXml = QString(), int dspMagicId = DspDeviceNode::MAGIC_ID21362);
private:
    std::unique_ptr<ZDspServer> m_server;
};

#endif // MOCKZDSP1D_H
