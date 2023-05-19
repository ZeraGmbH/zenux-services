#include "mt310s2systeminfo.h"
#include "micro-controller-io/atmelsysctrl.h"
#include "micro-controller-io/atmel.h"

Mt310s2SystemInfo::Mt310s2SystemInfo() :
    cSystemInfo()
{
    m_sSysCTRLVersion = "Unknown";
    getSystemInfo();
}

void Mt310s2SystemInfo::getSystemInfo()
{
    int rm = pAtmelSys->readCTRLVersion(m_sSysCTRLVersion);
    m_bRead = (rm == ZeraMControllerIo::cmddone);
}

bool Mt310s2SystemInfo::dataRead()
{
    return m_bRead && cSystemInfo::dataRead();
}

QString &Mt310s2SystemInfo::getSysCTRLVersion()
{
    return m_sSysCTRLVersion;
}
