#include "fileutils.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>

bool FileUtils::dirExistsForFileName(QString fullFileName)
{
    QFileInfo fi(fullFileName);
    QString dirName = fi.absolutePath();
    QDir dir(dirName);
    QString canonicalDirPath = dir.canonicalPath();
    if(!canonicalDirPath.isEmpty()) {
        QFileInfo dirFi(canonicalDirPath);
        return dirFi.isDir();
    }
    return false;
}

QString FileUtils::getExistingParentNonRootPath(const QString &path)
{
    if(!path.startsWith("/"))
        return QString();

    QDir dir(path);
    if(dir.exists())
        return dir.canonicalPath();

    if(!dir.makeAbsolute())
        return QString();
    if(dir.exists() && !dir.isRoot())
        return dir.canonicalPath();

    do
        dir.setPath(QDir::cleanPath(dir.filePath(QStringLiteral(".."))));
    while(!dir.exists() && !dir.isRoot());

    if(!dir.exists() || dir.isRoot())
        return QString();

    return dir.canonicalPath();
}

QString FileUtils::getLocalHostname()
{
    QFile file("/etc/hostname");
    if(!file.open(QFile::ReadOnly))
        return QString();
    return file.readAll().replace("\n", "");
}
