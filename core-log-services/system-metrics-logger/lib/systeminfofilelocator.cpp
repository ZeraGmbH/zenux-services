#include "systeminfofilelocator.h"
#include <QDir>

QString SystemInfoFileLocator::m_procStatusFileName = QStringLiteral("/proc/stat");
QString SystemInfoFileLocator::m_procMeminfoFileName = QStringLiteral("/proc/meminfo");
QString SystemInfoFileLocator::m_sysTempRootPath = QStringLiteral("/sys/class/thermal");
QString SystemInfoFileLocator::m_sysCpuPath = QStringLiteral("/sys/devices/system/cpu/cpufreq/");
QString SystemInfoFileLocator::m_procInterruptFileName = QStringLiteral("/proc/interrupts");
QString SystemInfoFileLocator::m_procDiskStatsName = QStringLiteral("/proc/diskstats");
QString SystemInfoFileLocator::m_procBasePath = QStringLiteral("/proc");
QList<QString> SystemInfoFileLocator::m_sysFreqPaths;

QString SystemInfoFileLocator::getProcStatusFileName()
{
    return m_procStatusFileName;
}

QString SystemInfoFileLocator::getProcMeminfoFileName()
{
    return m_procMeminfoFileName;
}

QString SystemInfoFileLocator::getSysTempRootPath()
{
    return m_sysTempRootPath;
}

QList<QString> SystemInfoFileLocator::getSysFreqPaths()
{
    if(m_sysFreqPaths.isEmpty()) {
        QDir cpuDir(m_sysCpuPath);
        cpuDir.setNameFilters(QStringList("policy*"));
        QFileInfoList infoList = cpuDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        for(auto &entry : infoList) {
            QString freqPath = entry.absoluteFilePath() + "/scaling_cur_freq";
            if(QFile::exists(freqPath))
                m_sysFreqPaths.append(freqPath);
            else
                qWarning("Frequency file can not be found at %s", qPrintable(freqPath));
        }

    }
    return m_sysFreqPaths;
}

QString SystemInfoFileLocator::getProcInterruptFileName()
{
    return m_procInterruptFileName;
}

QString SystemInfoFileLocator::getProcDiskStatsName()
{
    return m_procDiskStatsName;
}

QString SystemInfoFileLocator::getProcBasePath()
{
    return m_procBasePath;
}
