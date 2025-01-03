#include "systeminfofilelocator.h"
#include <QDir>

QString SystemInfoFileLocator::m_sysTempRootPath = QStringLiteral("/sys/class/thermal");
QString SystemInfoFileLocator::m_sysCpuPath = QStringLiteral("/sys/devices/system/cpu/cpufreq/");
QString SystemInfoFileLocator::m_procBasePath = QStringLiteral("/proc");
QList<QString> SystemInfoFileLocator::m_sysFreqPaths;

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

QString SystemInfoFileLocator::getProcBasePath()
{
    return m_procBasePath;
}

const QString SystemInfoFileLocator::getProcFileName(const QString &fileName)
{
    QString procDir = SystemInfoFileLocator::getProcBasePath();
    if(!procDir.endsWith("/"))
        procDir += "/";
    return QString("%1%2").arg(procDir, fileName);
}

const QString SystemInfoFileLocator::getProcFileName(int pid, const QString &fileName)
{
    QString procDir = SystemInfoFileLocator::getProcBasePath();
    if(!procDir.endsWith("/"))
        procDir += "/";
    return QString("%1%2/%3").arg(procDir).arg(pid).arg(fileName);
}
