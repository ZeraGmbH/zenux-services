#include "coredumpwatcher.h"

#include <QFileInfo>
#include <QDir>

CoreDumpWatcher::CoreDumpWatcher(QString coreDumpDir, QString outputDir, QList<int> userIdsToWatch):
    m_coreDumpDir(coreDumpDir),
    m_outputDir(outputDir),
    m_userIdsToWatch(userIdsToWatch)
{
}

void CoreDumpWatcher::startWatching()
{
    QDir dir;
    dir.mkpath(m_outputDir);
    QFile file(m_outputDir);
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                        QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther |
                        QFileDevice::ReadGroup | QFileDevice::ExeGroup);
    file.close();
    QDir coreDumpDir(m_coreDumpDir);
    if(!coreDumpDir.exists())
        qWarning("Input coredump dir %s, does not exist!", qPrintable(m_coreDumpDir));
    if(m_watcher.addPath(m_coreDumpDir)) {
        connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &CoreDumpWatcher::newCoreDumpFound);
        qInfo("Watching dir %s", qPrintable(m_coreDumpDir));
        newCoreDumpFound(m_coreDumpDir);
    }
    else
        qWarning("Cannot watch directory %s!", qPrintable(m_coreDumpDir));
}

void CoreDumpWatcher::newCoreDumpFound(QString path)
{
    QDir coreDumpDir(path);
    QFileInfoList filesInDir(coreDumpDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files));
    for(auto &entry:filesInDir) {
        int userId = extractUserId(entry.fileName());
        if(m_userIdsToWatch.contains(userId)) {
            QString absoluteFilePath = entry.absoluteFilePath();
            QString cmd = QString("mv %1 %2").arg(absoluteFilePath, m_outputDir + "/");
            if(system(qPrintable(cmd)) == 0) {
                if(fixPermissions(m_outputDir + "/" + entry.fileName())) {
                    qInfo("Successfully moved core file from %s to %s",
                          qPrintable(absoluteFilePath),qPrintable(m_outputDir));
                    QMetaObject::invokeMethod(this,
                                              "sigCoredumpMoved",
                                              Qt::QueuedConnection);
                }
            }
            else
                qWarning("Move from %s to %s failed", qPrintable(absoluteFilePath), qPrintable(m_outputDir));
        }
    }

}

bool CoreDumpWatcher::fixPermissions(QString filePath)
{
    QFile file(filePath);
    if(file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOther | QFileDevice::WriteOther |
                            QFileDevice::ReadGroup))
        return true;
    qWarning("Cannot fix permissions of file %s!", qPrintable(filePath));
    return false;
}

int CoreDumpWatcher::extractUserId(QString fileName)
{
    QStringList splitString = fileName.split(".");
    if(splitString.count() > 3)
        return splitString[2].toInt();
    qWarning("Cannot extractUserId from %s", qPrintable(fileName));
    return -1;
}
