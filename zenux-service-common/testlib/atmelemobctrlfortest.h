#ifndef ATMELEMOBCTRLFORTEST_H
#define ATMELEMOBCTRLFORTEST_H

#include <atmelemobctrl.h>

class AtmelEmobCtrlForTest : public AtmelEmobCtrl
{
public:
    AtmelEmobCtrlForTest(ZeraMcontrollerIoPtr i2cCtrl, QString devnode, quint8 adrMux, quint8 muxChannel, bool responding);
    virtual ~AtmelEmobCtrlForTest();
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    QString getDevnode();
    void setAdrCtrl(quint8 adrCtrl);
    quint8 getAdrCtrl();
    quint8 getAdrMux();
    quint8 getMuxChannel();
    void setDebuglevel(quint8 level);
    quint8 getDebuglevel();
    static int getInstanceCount();
private:
    QString m_devnode;
    quint8 m_adrCtrl;
    quint8 m_adrMux;
    quint8 m_muxChannel;
    quint8 m_debuglevel;
    bool m_responding;
    static int m_instanceCount;
};

#endif // ATMELEMOBCTRLFORTEST_H
