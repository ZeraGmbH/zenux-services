#ifndef MT310S2SYSTEMINFO_H
#define MT310S2SYSTEMINFO_H

#include "systeminfo.h"
#include <QString>

class Mt310s2SystemInfo : public cSystemInfo
{
public:
    Mt310s2SystemInfo();
    void getSystemInfo(); // here we fetch all system info
    bool dataRead(); // false if we were not able to read required information
    QString& getSysCTRLVersion();

private:
    bool m_bRead; // we keep in mind whether we could read the information
    QString m_sSysCTRLVersion;
};
#endif // MT310S2SYSTEMINFO_H
