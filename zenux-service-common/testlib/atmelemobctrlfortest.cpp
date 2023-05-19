#include "atmelemobctrlfortest.h"

int AtmelEmobCtrlForTest::m_instanceCount = 0;

AtmelEmobCtrlForTest::AtmelEmobCtrlForTest(ZeraMcontrollerIoPtr i2cCtrl, QString devnode, quint8 adrMux, quint8 muxChannel) :
    AtmelEmobCtrl(i2cCtrl, devnode, adrMux, muxChannel),
    m_devnode(devnode),
    m_adrMux(adrMux),
    m_muxChannel(muxChannel)
{
    m_instanceCount++;
}

AtmelEmobCtrlForTest::~AtmelEmobCtrlForTest()
{
    m_instanceCount--;
}

ZeraMControllerIo::atmelRM AtmelEmobCtrlForTest::readCTRLVersion(QString &answer)
{
    answer = "EMOB test";
    return ZeraMControllerIo::cmddone;
}

QString AtmelEmobCtrlForTest::getDevnode()
{
    return m_devnode;
}

void AtmelEmobCtrlForTest::setAdrCtrl(quint8 adrCtrl)
{
    m_adrCtrl = adrCtrl;
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

void AtmelEmobCtrlForTest::setDebuglevel(quint8 level)
{
    m_debuglevel = level;
}

quint8 AtmelEmobCtrlForTest::getDebuglevel()
{
    return m_debuglevel;
}

int AtmelEmobCtrlForTest::getInstanceCount()
{
    return m_instanceCount;
}
