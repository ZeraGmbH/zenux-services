#ifndef TESTHOTPLUGI2CCTRLCOMMONVERSIONS_H
#define TESTHOTPLUGI2CCTRLCOMMONVERSIONS_H

#include "abstracti2ccontrollers.h"
#include "i2csettings.h"

class TestHotplugI2cCtrlCommonVersions : public AbstractI2cCtrlCommonVersions
{
public:
    TestHotplugI2cCtrlCommonVersions(I2cSettings *i2cSettings, quint8 muxChannel, bool responding, int &instanceCount);
    ~TestHotplugI2cCtrlCommonVersions();
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

#endif // TESTHOTPLUGI2CCTRLCOMMONVERSIONS_H
