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
    if(m_watcher.addPath(m_coreDumpDir)) {
        connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &CoreDumpWatcher::newCoreDumpFound);
    }
}

void CoreDumpWatcher::newCoreDumpFound(QString path)
{
    QDir coreDumpDir(path);
    QFileInfoList filesInDir(coreDumpDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files));
    if(!filesInDir.isEmpty())
        for(auto &entry:filesInDir){
            int userId = extractUserId(entry.fileName());
            if(m_userIdsToWatch.contains(userId))
                if(fixPermissions(entry.absoluteFilePath())){
                    QString cmd = QString("mv %1 %2").arg(entry.absoluteFilePath(), m_outputDir + "/");
                    if(system(qPrintable(cmd)) == 0)
                        emit sigCoredumpCreated();
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
    return false;
}

int CoreDumpWatcher::extractUserId(QString fileName)
{
    int fileUserId = -1;
    QStringList splitString = fileName.split(".");
    if(splitString.count() > 3)
        fileUserId = splitString[2].toInt();

    return fileUserId;
}
