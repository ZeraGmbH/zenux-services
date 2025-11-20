#ifndef TESTHOTPLUGI2CCTRLCOMMONINFO_H
#define TESTHOTPLUGI2CCTRLCOMMONINFO_H

#include "abstractalli2ccontrollers.h"
#include "i2csettings.h"

class TestHotplugI2cCtrlCommonInfo : public AbstractI2cCtrlCommonInfo
{
public:
    TestHotplugI2cCtrlCommonInfo(I2cSettings *i2cSettings, quint8 muxChannel);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString&) override;
    virtual ZeraMControllerIo::atmelRM readPCBInfo(QString&) override;

    QString getDevnode();
    quint8 getAdrCtrl();
    quint8 getAdrMux();
    quint8 getMuxChannel();

private:
    I2cSettings *m_i2cSettings;
    quint8 m_muxChannel;
};

#endif // TESTHOTPLUGI2CCTRLCOMMONINFO_H
