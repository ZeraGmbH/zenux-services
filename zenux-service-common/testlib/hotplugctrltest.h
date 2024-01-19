#ifndef HOTPLUGCTRLTEST_H
#define HOTPLUGCTRLTEST_H

#include "abstracti2ccontrollers.h"
#include "i2csettings.h"

class HotPlugCtrlTest : public AbstractI2cCtrlCommonVersions
{
public:
    HotPlugCtrlTest(I2cSettings *i2cSettings, quint8 muxChannel, bool responding, int &instanceCount);
    ~HotPlugCtrlTest();
    ZeraMControllerIo::atmelRM readCTRLVersion(QString&) override;
    virtual ZeraMControllerIo::atmelRM readPCBVersion(QString&) override;

    QString getDevnode();
    quint8 getAdrCtrl();
    quint8 getAdrMux();
    quint8 getMuxChannel();

private:
    I2cSettings *m_i2cSettings;
    quint8 m_muxChannel;
    bool m_responding;
    int &m_instanceCount;
};

#endif // HOTPLUGCTRLTEST_H
