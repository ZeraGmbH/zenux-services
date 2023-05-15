#include "atmelctrlfactoryfortest.h"
#include "atmelemobctrlfortest.h"

void AtmelCtrlFactoryForTest::enableTest()
{
    m_emobCreateFunction = [](QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel)
    {
        return std::make_shared<AtmelEmobCtrlForTest>(devnode, adr, ctrlChannelForMux, debuglevel);
    };
}
