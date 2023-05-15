#include "atmelemobctrlfortest.h"

int AtmelEmobCtrlForTest::m_instanceCount = 0;

AtmelEmobCtrlForTest::AtmelEmobCtrlForTest(QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel) :
    AtmelEmobCtrl(devnode, adr, ctrlChannelForMux, debuglevel),
    m_devnode(devnode),
    m_adr(adr),
    m_ctrlChannelForMux(ctrlChannelForMux),
    m_debuglevel(debuglevel)
{
    m_instanceCount++;
}

AtmelEmobCtrlForTest::~AtmelEmobCtrlForTest()
{
    m_instanceCount--;
}

QString AtmelEmobCtrlForTest::getDevnode()
{
    return m_devnode;
}

quint8 AtmelEmobCtrlForTest::getAdr()
{
    return m_adr;
}

quint8 AtmelEmobCtrlForTest::getCtrlChannelForMux()
{
    return m_ctrlChannelForMux;
}

quint8 AtmelEmobCtrlForTest::getDebuglevel()
{
    return m_debuglevel;
}

int AtmelEmobCtrlForTest::getInstanceCount()
{
    return m_instanceCount;
}
