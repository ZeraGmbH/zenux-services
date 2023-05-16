#include "atmelctrlfactoryfortest.h"
#include "atmelemobctrlfortest.h"

void AtmelCtrlFactoryForTest::enableTest()
{
    m_emobCreateFunction = [](QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 channelMux, quint8 debuglevel)
    {
        return std::make_shared<AtmelEmobCtrlForTest>(devnode, adrCtrl, adrMux, channelMux, debuglevel);
    };
}
