#ifndef MOCKMT310S2DFULL_H
#define MOCKMT310S2DFULL_H

#include "mt310s2d.h"

class MockMt310s2dFull : public cMT310S2dServer
{
    Q_OBJECT
public:
    MockMt310s2dFull(std::shared_ptr<SettingsForDeviceServer> settings, AtmelCtrlFactoryInterfacePrt ctrlFactory);
};

#endif // MOCKMT310S2DFULL_H
