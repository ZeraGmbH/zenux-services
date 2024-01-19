#include "mt310s2systeminfo.h"

Mt310s2SystemInfo::Mt310s2SystemInfo(FactoryControllerAbstractPtr ctrlFactory) :
    SystemInfo(ctrlFactory),
    m_ctrlFactory(ctrlFactory)
{
    m_sSysCTRLVersion = m_sSysPCBVersion = "Unknown";
    getSystemInfo();
}

void Mt310s2SystemInfo::getSystemInfo()
{
    I2cCtrlCommonInfoPtrUnique controller = m_ctrlFactory->getCommonVersionController(AbstractFactoryI2cCtrl::CTRL_TYPE_SYSTEM);
    int rm = controller->readCTRLVersion(m_sSysCTRLVersion);
    rm |= controller->readPCBInfo(m_sSysPCBVersion);
    m_bRead = (rm == ZeraMControllerIo::cmddone);
}

bool Mt310s2SystemInfo::dataRead()
{
    return m_bRead && SystemInfo::dataRead();
}

QString Mt310s2SystemInfo::getSysCTRLVersion()
{
    return m_sSysCTRLVersion;
}

QString Mt310s2SystemInfo::getSysPCBVersion()
{
    return m_sSysPCBVersion;
}
