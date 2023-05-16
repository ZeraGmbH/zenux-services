#ifndef ATMELEMOBCTRLFORTEST_H
#define ATMELEMOBCTRLFORTEST_H

#include <atmelemobctrl.h>

class AtmelEmobCtrlForTest : public AtmelEmobCtrl
{
public:
    AtmelEmobCtrlForTest(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel);
    virtual ~AtmelEmobCtrlForTest();
    QString getDevnode();
    quint8 getAdrCtrl();
    quint8 getAdrMux();
    quint8 getMuxChannel();
    quint8 getDebuglevel();
    static int getInstanceCount();
private:
    QString m_devnode;
    quint8 m_adrCtrl;
    quint8 m_adrMux;
    quint8 m_muxChannel;
    quint8 m_debuglevel;
    static int m_instanceCount;
};

#endif // ATMELEMOBCTRLFORTEST_H
