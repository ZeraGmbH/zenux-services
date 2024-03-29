#ifndef MT310S2SYSTEMINFO_H
#define MT310S2SYSTEMINFO_H

#include "systeminfo.h"
#include <memory>
#include <QString>

class Mt310s2SystemInfo : public SystemInfo
{
public:
    Mt310s2SystemInfo(AbstractFactoryI2cCtrlPtr ctrlFactory);
    void getSystemInfo(); // here we fetch all system info
    bool dataRead(); // false if we were not able to read required information
    virtual QString getSysCTRLVersion();
    virtual QString getSysPCBVersion();

private:
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    bool m_bRead; // we keep in mind whether we could read the information
    QString m_sSysCTRLVersion;
    QString m_sSysPCBVersion;
};
#endif // MT310S2SYSTEMINFO_H
