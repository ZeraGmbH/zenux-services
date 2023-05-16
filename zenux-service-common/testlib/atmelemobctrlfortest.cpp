#include "atmelemobctrlfortest.h"

int AtmelEmobCtrlForTest::m_instanceCount = 0;

AtmelEmobCtrlForTest::AtmelEmobCtrlForTest(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel) :
    AtmelEmobCtrl(devnode, adrCtrl, adrMux, muxChannel, debuglevel),
    m_devnode(devnode),
    m_adrCtrl(adrCtrl),
    m_adrMux(adrMux),
    m_muxChannel(muxChannel),
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

quint8 AtmelEmobCtrlForTest::getAdrCtrl()
{
    return m_adrCtrl;
}

quint8 AtmelEmobCtrlForTest::getAdrMux()
{
    return m_adrMux;
}

quint8 AtmelEmobCtrlForTest::getMuxChannel()
{
    return m_muxChannel;
}

quint8 AtmelEmobCtrlForTest::getDebuglevel()
{
    return m_debuglevel;
}

int AtmelEmobCtrlForTest::getInstanceCount()
{
    return m_instanceCount;
}
