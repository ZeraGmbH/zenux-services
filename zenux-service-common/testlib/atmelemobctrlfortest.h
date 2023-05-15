#ifndef ATMELEMOBCTRLFORTEST_H
#define ATMELEMOBCTRLFORTEST_H

#include <atmelemobctrl.h>

class AtmelEmobCtrlForTest : public AtmelEmobCtrl
{
public:
    AtmelEmobCtrlForTest(QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel);
    virtual ~AtmelEmobCtrlForTest();
    QString getDevnode();
    quint8 getAdr();
    quint8 getCtrlChannelForMux();
    quint8 getDebuglevel();
    static int getInstanceCount();
private:
    QString m_devnode;
    quint8 m_adr;
    quint8 m_ctrlChannelForMux;
    quint8 m_debuglevel;
    static int m_instanceCount;
};

#endif // ATMELEMOBCTRLFORTEST_H
