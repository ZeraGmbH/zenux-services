#ifndef MT310S2SYSTEMINFO_H
#define MT310S2SYSTEMINFO_H

#include "systeminfo.h"
#include <memory>
#include <QString>

class Mt310s2SystemInfo : public cSystemInfo
{
public:
    Mt310s2SystemInfo(AtmelCtrlFactoryInterfacePrt ctrlFactory);
    void getSystemInfo(); // here we fetch all system info
    bool dataRead(); // false if we were not able to read required information
    QString& getSysCTRLVersion();
    QString& getSysPCBVersion();

private:
    AtmelCtrlFactoryInterfacePrt m_ctrlFactory;
    bool m_bRead; // we keep in mind whether we could read the information
    QString m_sSysCTRLVersion;
    QString m_sSysPCBVersion;
};
#endif // MT310S2SYSTEMINFO_H
