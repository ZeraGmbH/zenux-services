#include "mt310s2systeminfo.h"

Mt310s2SystemInfo::Mt310s2SystemInfo(std::shared_ptr<AtmelCtrlSystem> systemController) :
    cSystemInfo(),
    m_systemController(systemController)
{
    m_sSysCTRLVersion = m_sSysPCBVersion = "Unknown";
    getSystemInfo();
}

void Mt310s2SystemInfo::getSystemInfo()
{
    int rm = m_systemController->readCTRLVersion(m_sSysCTRLVersion);
    rm |= m_systemController->readPCBVersion(m_sSysPCBVersion);
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

QString &Mt310s2SystemInfo::getSysPCBVersion()
{
    return m_sSysPCBVersion;
}
