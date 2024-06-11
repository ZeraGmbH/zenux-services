#include "onofffilewatcher.h"
#include <QFile>
#include <QDir>

static const char* onOffPath = "/tmp/onofffilewatcher/";
static const char* onOffFile = "log_is_on";

OnOffFileWatcher::OnOffFileWatcher()
{
    QDir dir;
    dir.mkpath(onOffPath);

    m_dirWatcher.addPath(onOffPath);
    connect(&m_dirWatcher, &QFileSystemWatcher::directoryChanged,
            this, &OnOffFileWatcher::onDirectoryChanged);
}

bool OnOffFileWatcher::isOn()
{
    return QFile::exists(getOnOffFullPath());
}

QString OnOffFileWatcher::getOnOffFullPath()
{
    QString fullPath = getOnOffDir() + QString(onOffFile);
    return fullPath;
}

QString OnOffFileWatcher::getOnOffDir()
{
    return onOffPath;
}

void OnOffFileWatcher::onDirectoryChanged()
{
    emit sigOnOffChangeDetected();
}
